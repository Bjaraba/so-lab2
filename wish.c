#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char *exit_str = "exit";
char *path = "/bin";

void print_arr(char **arr);
void handle_input(char *input);
void handle_extern_commands(char **args);
void extern_command(char *command, char **args);

int main(int argc, char const *argv[])
{
  while (1)
  {
    printf("wish> ");
    char *line;
    size_t len = 0;

    getline(&line, &len, stdin);
    line[strcspn(line, "\n")] = '\0'; // remove the \n character at the final of the input string
    int leave = strcmp(line, exit_str);
    if (leave == 0)
    {
      printf("good bye!\n");
      free(line);
      exit(0);
    }
    else
    {
      handle_input(line);
    }
  }

  return 0;
}

void handle_input(char *input)
{
  if (strlen(input) == 0)
  {
    return; // skip void lines
  }
  char *input_copy = malloc(strlen(input) + 1);
  strcpy(input_copy, input); // Copy input

  char path[256];
  strcpy(path, "/bin/");

  char **args = NULL;
  char *item;
  int i = 0;

  while ((item = strsep(&input_copy, " ")) != NULL)
  {
    if (strlen(item) == 0)
    {
      continue; // avoid void words
    }
    args = realloc(args, (i + 2) * sizeof(char *));
    args[i] = malloc(strlen(item) + 1);
    strcpy(args[i], item);
    args[i + 1] = NULL;
    i++;
  }

  if (args == NULL || args[0] == NULL)
  {
    char error_message[30] = "command not found\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
  }
  else
  {
    strcat(path, args[0]);
    int available = access(path, X_OK);
    if (available == -1)
    {
      char error_message[30] = "command not found\n";
      write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else
    {
      // Replace args[0] with the full path for execv
      free(args[0]);
      args[0] = malloc(strlen(path) + 1);
      strcpy(args[0], path);
      
      handle_extern_commands(args);
    }
  }

  // Free memory
  if (args != NULL)
  {
    for (int j = 0; args[j] != NULL; j++)
    {
      free(args[j]);
    }
    free(args);
  }
  free(input_copy);
}

void handle_extern_commands(char **args)
{
  int pid = fork();
  if (pid < 0)
  {
    printf("pid negative\n");
    char error_message[30] = "An error has ocurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
  }
  else if (pid == 0)
  {
    if (execv(args[0], args) == -1)
    {
      printf("execv error\n");
      char error_message[30] = "An error has ocurred\n";
      write(STDERR_FILENO, error_message, strlen(error_message));
    }
  }
  else
  {
    wait(NULL);
  }
}

void print_arr(char **arr)
{
  int count = 0;
  char *item;
  while ((item = arr[count]) != NULL)
  {
    printf("item %d: %s\n", count, item);
    count++;
  }
}
