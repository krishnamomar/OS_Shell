#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int ERR_STATUS = 0;

void parse_mode(char *raw_mode, int flag_verbose, mode_t flag_mode)
{
  mode_t mode = (mode_t)strtol(raw_mode, NULL, 8);
  if (
      (errno == ERANGE && (mode == LONG_MAX || mode == LONG_MIN)) ||
      (mode == 0))
  {
    fprintf(stderr, "mkdir: invalid mode '%s'", raw_mode);
    exit(EXIT_FAILURE);
  }

  flag_mode = mode;
}

int main(int argc, char *argv[])
{
  char *cwd = argv[0];

  char *paths[argc];
  int path_count = 0;
  int flag_verbose = 0;
  mode_t flag_mode = (mode_t) 0;

  for (int i = 1; i < argc; i++)
  {
    char *arg = argv[i];

    // Double dash keyword options
    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-')
    {
      if (strcmp(arg, "--verbose") == 0) flag_verbose = 1;
      else
      {
        char keyword[8];
        memcpy(keyword, arg, 7);
        keyword[7] = '\0';

        if (strcmp(keyword, "--mode=") == 0)
        {
          char *raw_mode = &arg[7];
          parse_mode(raw_mode, flag_verbose, flag_mode);
        }
        else {
          fprintf(stderr, "mkdir: unrecognized option '%s'\n", arg);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // Single dash keyword letter options
    if (strlen(arg) > 1 && arg[0] == '-')
    {
      if (strcmp(arg, "-v") == 0) flag_verbose = 1;
      else
      {
        char keyword[4];
        memcpy(keyword, arg, 3);
        keyword[3] = '\0';

        if (strcmp(keyword, "-m=") == 0)
        {
          char *raw_mode = &arg[7];
          parse_mode(raw_mode, flag_verbose, flag_mode);
        }
        else
        {
          fprintf(stderr, "mkdir: invalid option -- '%c'\n", arg[1]);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    paths[path_count++] = arg;
  }

  if (path_count == 0)
  {
    fprintf(stderr, "mkdir: missing operand\n");
    exit(1);
  }

  for (int i = 0; i < path_count; i++)
  {
    char *path = paths[i];

    if (mkdir(path, flag_mode ? flag_mode : 0755) == -1)
    {
      switch (errno)
      {
        case EACCES:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': Permission denied\n",
            path
          );
          break;

        case EEXIST:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': File exits\n",
            path
          );
          break;

        case ENOENT:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': No such file or directory\n",
            path
          );
      }

      ERR_STATUS = 1;
      continue;
    }

    if (flag_verbose){
      printf("mkdir: created directory '%s'\n", path);
    }
  }

  return 0;
}