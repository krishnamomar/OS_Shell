#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int echo(int argc, char *argv[])
{
  char *tokens[argc];
  int token_count = 0;
  int parse_options = 1;

  int flag[2] = {0, 0};

  for (int i = 0; i < argc; i++)
  {
    char *arg = argv[i];

    // Single dash keyword letter options
    if (parse_options && strlen(arg) > 1 && arg[0] == '-')
    {
      int is_token = 0;
      int buffer[2] = {0, 0};
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'n') buffer[0] = 1;
        else if (letter == 'e') buffer[1] = 1;
        else {
          is_token = 1;
          buffer[0] = 0;
          buffer[1] = 0;
          break;
        }
      }
      flag[0] |= buffer[0];
      flag[1] |= buffer[1];
      if (!is_token)
        continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    tokens[token_count++] = arg;
    parse_options = 0;
  }

  for (int i = 0; i < token_count; i++)
    printf("%s ", tokens[i]);

  if (!flag[0])
    printf("\n");

  return 0;
}