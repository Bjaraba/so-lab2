#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char error_message[30] = "An error has occurred\n";

// PATH dinámico
char *paths[100];
int path_count = 0;

// ====================== UTIL ======================
void print_error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// ====================== BUILT-INS ======================
void builtin_cd(char **args, int count) {
    if (count != 2) {
        print_error();
        return;
    }
    if (chdir(args[1]) != 0) {
        print_error();
    }
}

void builtin_path(char **args, int count) {
    for (int i = 0; i < path_count; i++) {
        free(paths[i]);
    }

    path_count = 0;

    for (int i = 1; i < count; i++) {
        paths[path_count++] = strdup(args[i]);
    }
}

// ====================== EXEC ======================
void execute_command(char **args) {
    if (path_count == 0) {
        print_error();
        exit(1);
    }

    for (int i = 0; i < path_count; i++) {
        char fullpath[256];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[i], args[0]);

        if (access(fullpath, X_OK) == 0) {
            execv(fullpath, args);
        }
    }

    print_error();
    exit(1);
}

// ====================== PARSE ======================
char **parse_args(char *line, int *count) {
    char **args = NULL;
    char *token;
    int i = 0;

    while ((token = strsep(&line, " \t\n")) != NULL) {
        if (strlen(token) == 0) continue;

        args = realloc(args, sizeof(char*) * (i + 2));
        args[i] = strdup(token);
        args[i + 1] = NULL;
        i++;
    }

    *count = i;
    return args;
}

// ====================== REDIRECCIÓN ======================
int handle_redirection(char *cmd, char **clean_cmd, char **outfile) {
    char *redirect = strchr(cmd, '>');
    if (!redirect) return 0;

    // más de un >
    if (strchr(redirect + 1, '>') != NULL) return -1;

    *redirect = '\0';
    redirect++;

    // quitar espacios iniciales
    while (*redirect == ' ' || *redirect == '\t') redirect++;

    if (*redirect == '\0') return -1;

    char *file = redirect;

    // quitar salto de línea
    file[strcspn(file, "\n")] = '\0';

    // quitar espacios finales
    int len = strlen(file);
    while (len > 0 && (file[len - 1] == ' ' || file[len - 1] == '\t')) {
        file[len - 1] = '\0';
        len--;
    }

    // validar un solo archivo
    char *space = strpbrk(file, " \t");
    if (space != NULL) {
        *space = '\0';
        space++;

        while (*space == ' ' || *space == '\t') space++;
        if (*space != '\0') return -1;
    }

    // limpiar comando
    while (*cmd == ' ' || *cmd == '\t') cmd++;
    if (*cmd == '\0') return -1;

    *outfile = strdup(file);
    *clean_cmd = strdup(cmd);

    return 1;
}

// ====================== MAIN ======================
int main(int argc, char *argv[]) {

    // PATH inicial
    paths[0] = strdup("/bin");
    path_count = 1;

    FILE *input = stdin;

    // modo batch
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            print_error();
            exit(1);
        }
    } 
    else if (argc > 2) {
        print_error();
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;

    while (1) {

        if (input == stdin) {
            printf("wish> ");
            fflush(stdout);
        }

        if (getline(&line, &len, input) == -1) {
            exit(0);
        }

        char *saveptr;
        char *cmd = strtok_r(line, "&", &saveptr);

        int children[100];
        int child_count = 0;

        while (cmd != NULL) {

            // ?? FIX CLAVE: limpiar ANTES
            cmd[strcspn(cmd, "\n")] = '\0';
            while (*cmd == ' ' || *cmd == '\t') cmd++;

            char *clean_cmd = NULL;
            char *outfile = NULL;

            int redir = handle_redirection(cmd, &clean_cmd, &outfile);

            if (redir == -1) {
                print_error();
                cmd = strtok_r(NULL, "&", &saveptr);
                continue;
            }

            char *use_cmd = (redir ? clean_cmd : cmd);

            int argc_cmd;
            char **args = parse_args(use_cmd, &argc_cmd);

            if (argc_cmd == 0) {
                cmd = strtok_r(NULL, "&", &saveptr);
                continue;
            }

            // ===== BUILT-INS =====
            if (strcmp(args[0], "exit") == 0) {
                if (argc_cmd != 1) print_error();
                else exit(0);
            } 
            else if (strcmp(args[0], "cd") == 0) {
                builtin_cd(args, argc_cmd);
            } 
            else if (strcmp(args[0], "path") == 0) {
                builtin_path(args, argc_cmd);
            } 
            else {
                int pid = fork();

                if (pid < 0) {
                    print_error();
                } 
                else if (pid == 0) {

                    if (redir) {
                        int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        if (fd < 0) {
                            print_error();
                            exit(1);
                        }

                        dup2(fd, STDOUT_FILENO);
                        dup2(fd, STDERR_FILENO);
                        close(fd);
                    }

                    execute_command(args);
                } 
                else {
                    children[child_count++] = pid;
                }
            }

            cmd = strtok_r(NULL, "&", &saveptr);
        }

        for (int i = 0; i < child_count; i++) {
            waitpid(children[i], NULL, 0);
        }
    }

    return 0;
}
