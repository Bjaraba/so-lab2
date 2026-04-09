#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *exit_str = "exit";

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
    printf("comparation between %s with %s result %d\n", line, exit_str, leave);
    if (leave == 0)
    {
      printf("good bye!\n");
      free(line);
      exit(0);
    }
  }

  return 0;
}
