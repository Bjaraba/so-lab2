#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char *exit_str = "exit";
char *path = "/bin";

void print_arr(char **arr);
void handle_input(char *input);
void handle_extern_commands(char *command, char **args);
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

  char *command = NULL;
  char **args = NULL;
  char *item;
  int i = 0;

  while ((item = strsep(&input_copy, " ")) != NULL)
  {
    if (strlen(item) == 0)
    {
      continue; // avoid void words
    }
    if (i == 0)
    {
      command = malloc(strlen(item) + 1);
      strcpy(command, item); // first word is the command
      i++;
      continue;
    }
    args = realloc(args, (i + 1) * sizeof(char *));
    args[i - 1] = malloc(strlen(item) + 1);
    strcpy(args[i - 1], item);
    args[i] = NULL;
    i++;
  }

  if (command == NULL)
  {
    printf("comando no disponible");
  }
  else
  {
    printf("commando: %s\n", command);
    strcat(path, command);
    int available = access(path, X_OK);
    if (available == -1)
    {
      printf("el comando no esta disponible\n");
    }
    else
    {
      handle_extern_commands(command, args);
    }
  }

  // Free memory
  if (command != NULL)
  {
    free(command);
  }
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

void handle_extern_commands(char *command, char **args)
{
  int pid = fork();
  if (pid < 0)
  {
    printf("ha ocurrido un error\n");
  }
  else if (pid == 0)
  {
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