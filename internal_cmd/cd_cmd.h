#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../path.h"

WD working_path;

int parse_cd(char *targetA, char *targetR)
{
  char *copy = strdup(targetR);
  char *ptr = copy;
  for (int i = 0; copy[i]; i++)
  {
    *ptr++ = targetR[i];
    if (targetR[i] == '/')
    {
      i++;
      while (targetR[i] == '/') i++;
      i--;
    }
  }
  *ptr = '\0';
  strcpy(targetR, copy);

  if (targetR[0] == '/')
  {
    strcpy(targetA, targetR);
    targetR[0] = '\0';
  }
  else
  {
    if (targetA[strlen(targetA) - 1] != '/')
      strcat(targetA, "/");
    strcat(targetA, targetR);
  }

  if (strcmp(targetA, "/") == 0)
    return 0;

  char new_path[MAX_INP_SIZE] = "/";

  char *saveptr;
  char *delim = "/";
  char *token = strtok_r(targetA, delim, &saveptr);

  do
  {
    if (strcmp(token, ".") == 0) continue;
    else if (strcmp(token, "..") == 0)
    {
      if (strcmp(new_path, "/") == 0) continue;
      int index = strlen(new_path) - 1;
      while (new_path[index] != '/' && index >= 0) index--;
      new_path[index] = '\0';
    }
    else
    {
      if (new_path[strlen(new_path) - 1] != '/')
        strcat(new_path, "/");
      strcat(new_path, token);
    }

    if (!strlen(new_path))
      strcpy(new_path, "/");

    struct stat statbuf;
    if (stat(new_path, &statbuf) != 0) return 1;
    if (!S_ISDIR(statbuf.st_mode)) return 2;
  }
  while (token = strtok_r(NULL, delim, &saveptr));

  strcpy(targetA, new_path);
  return 0;
}

int cd(int argc, char *argv[])
{
  int print_physical = 0;
  int arg_count = 0;
  char *targetR = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char *targetSave = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char targetA[MAX_INP_SIZE];
  strcpy(targetA, working_path.current);

  for (int i = 0; i < argc; i++)
  {
    if (arg_count > 0)
    {
      fprintf(stderr, "pikachu: cd: too many arguments\n");
      
      free(targetR);
      free(targetSave);
      return 1;
    }

    char *arg = argv[i];

    // Single dash keyword letter options
    if (strlen(arg) > 1 && arg[0] == '-')
    {
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'P') print_physical = 1;
        else if (letter == 'L') print_physical = 0;
        else
        {
          fprintf(stderr, "pikachu: cd: -'%c': invalid option\n", letter);
          
          free(targetR);
          free(targetSave);
          return 1;
        }
      }
      continue;
    }

    targetR = strdup(arg);
    targetSave = strdup(arg);
    arg_count++;
  }

  if (strcmp(targetR, "-") == 0)
    strcpy(targetA, working_path.previous);
  else
  {
    int err;
    if (err = parse_cd(targetA, targetR))
    {
      switch (err)
      {
        case 1:
          fprintf(stderr, "pikachu: cd: no such file or directory: %s\n", targetSave);
          break;
        case 2:
          fprintf(stderr, "pikachu: cd: not a directory: %s\n", targetSave);
      }
      
      free(targetR);
      free(targetSave);
      return 1;
    }
  }

  if (print_physical)
    strcpy(targetA, realpath(targetA, NULL));

  struct stat statbuf;
  if (stat(targetA, &statbuf) != 0)
  {
    fprintf(stderr, "pikachu: cd: no such file or directory: %s\n", targetSave);
    
    free(targetR);
    free(targetSave);
    return 1;
  }
  if (!S_ISDIR(statbuf.st_mode))
  {
    fprintf(stderr, "pikachu: cd: not a directory: %s\n", targetSave);
    
    free(targetR);
    free(targetSave);
    return 1;
  }

  strcpy(working_path.previous, working_path.current);
  strcpy(working_path.current, targetA);
  chdir(working_path.current);

  
  free(targetR);
  free(targetSave);
  return 0;
}