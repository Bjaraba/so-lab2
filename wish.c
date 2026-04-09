#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *exit_str = "exit";
char *path = "/bin";

void handle_input(char *input);
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
  char path[256];
  strcpy(path, "/bin/");
  char *token;
  token = strsep(&input, " ");

  if (token == NULL)
  {
    printf("comando no disponible");
  }

  strcat(path, token);
  int available = access(path, X_OK);
  if (available == 0)
  {
    printf("el ejecutable: '%s' esta dispolible\n", path);
  }
  else
  {
    printf("comando no disponible\n");
  }
}
