#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int ERR_STATUS = 0;

void stdin_loop(int *line_number, int flag[])
{
  while(1)
  {
    char string[200];
    scanf("%s", string);
    if (flag[1]){
      int line_x = (intptr_t) ++(*line_number);
      int number_length = floor(log10(line_x)) + 1;
      int padsize = 6 - number_length;

      for (int i = 0; i < padsize; i++)
        printf(" ");

      printf("%d  ", line_x);
    }
    printf("%s%s\n", string, flag[0] ? "$" : "");

    if (feof(stdin))
      break;
  }
}

int main(int argc, char *argv[])
{
  char *paths[argc];
  int path_count = 0;
  int flag[] = { 0, 0 };

  for (int i = 1; i < argc; i++)
  {
    char *arg = argv[i];

    // Double dash keyword options
    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-')
    {
      if (strcmp(arg, "--show-ends") == 0) flag[0] = 1;
      else if (strcmp(arg, "--number") == 0) flag[1] = 1;
      else
      {
        fprintf(stderr, "cat: unrecognized option '%s'\n", arg);
        exit(EXIT_FAILURE);
      }
      continue;
    }

    // Single dash keyword letter options
    if (strlen(arg) > 1 && arg[0] == '-')
    {
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'E') flag[0] = 1;
        else if (letter == 'n') flag[1] = 1;
        else
        {
          fprintf(stderr, "cat: invalid option -- '%c'\n", letter);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    paths[path_count++] = arg;
  }

  int line_number = 0;
  int new_line = 1;

  // Copy standard input to standard output
  if (path_count == 0)
  {
    stdin_loop(&line_number, flag);
    return 0;
  }

  for (int i = 0; i < path_count; i++)
  {
    char *path = paths[i];

    if (strcmp(path, "-") == 0)
    {
      stdin_loop(&line_number, flag);
      continue;
    }

    struct stat path_stat;
    stat(path, &path_stat);
    if (!S_ISREG(path_stat.st_mode))
    {
      fprintf(stderr, "cat: %s: Is a directory\n", path);
      ERR_STATUS = 1;
      continue;
    }

    FILE *fd = fopen(path, "r");
    if (fd == NULL)
    {
      fprintf(stderr, "cat: %s: No such file or directory\n", path);
      ERR_STATUS = 1;
      continue;
    }

    int c;

    while ((c = fgetc(fd)) && !feof(fd))
    {
      if (new_line && flag[1]){
        int line_x = ++line_number;
        int number_length = floor(log10(line_x)) + 1;
        int padsize = 6 - number_length;

        for (int i = 0; i < padsize; i++)
          printf(" ");

        printf("%d  ", line_x);
      }

      new_line = 0;

      if (c == '\n')
      {
        new_line = 1;
        if (flag[0])
          printf("$");
      }

      printf("%c", c);
    }

    fclose(fd);
  }

  return ERR_STATUS;
}