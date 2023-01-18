#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>
#include <pthread.h>

#include "path.h"
#include "internal_cmd/cd_cmd.h"
#include "internal_cmd/echo_cmd.h"
#include "internal_cmd/pwd_cmd.h"

#include "extra_cmd/exit_cmd.h"

#define HISTORY_FILE "/.main_shell_history"
/*
`;-.          ___,
  `.`\_...._/`.-"`
    \        /      ,
    /()   () \    .' `-._
   |)  .    ()\  /   _.'
   \  -'-     ,; '. <
    ;.__     ,;|   > \
   / ,    / ,  |.-'.-'
  (_/    (_/ ,;|.<`
    \    ,     ;-`
     >   \    /
    (_,-'`> .'
         (_,'
*/

/*
       ,___          .-;'
       `"-.`\_...._/`.`
    ,      \        /
 .-' ',    / ()   ()\
`'._   \  /()    .  (|
    > .' ;,     -'-  /
   / <   |;,     __.;
   '-.'-.|  , \    , \
      `>.|;, \_)    \_)
       `-;     ,    /
          \    /   <
           '. <`'-,_)
            '._)
*/

int print_welcome_note(){

    printf("Welcome to PIKACHU!!\n");
    printf("-------------------------------------\n");
    printf("A shell made by Krishnam Omar (krishnam20309@iiitd.ac.in)\n");
    printf("-------------------------------------\n");
    printf("This shell takes following commands: \n");
    printf("Internal Commands are: \n");
    printf("    1. cd\n");
    printf("    2. echo\n");
    printf("    4. pwd\n");
    printf("External Commands are: \n");
    printf("    1. ls\n");
    printf("    2. cat\n");
    printf("    3. date\n");
    printf("    4. rm\n");
    printf("    5. mkdir\n");
    printf("Extra Commands are: \n");
    printf("    1. exit\n");
    printf("    2. help\n");
    printf("-------------------------------------\n");
    printf("-------------------------------------\n");

    return 0;
}


int fork_and_exec(char cmd[], int argc, char *argv[]){

    int child_pid = fork();
    if (child_pid < 0){
        fprintf(stderr,"ERR! Could not fork parent process\n");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0){
        char abs_cmd[MAX_INP_SIZE];
        strcpy(abs_cmd, working_path.start);
        strcat(abs_cmd, cmd);

        int err = execvp(abs_cmd, argv) == -1;

        char *errmsg = (char *) calloc(200, sizeof(char));
        snprintf(errmsg, 200, "%s", argv[0]);
        perror(errmsg);

        free(errmsg);
        _exit(err);
    }
    else {
        int exit_status;
        waitpid(child_pid, &exit_status, 0);
        return exit_status != 0;
    }
}


void *system_runner( void *ptr ){
    char *message;
    message = (char *) ptr;
    // printf("%s \n", message);
    system(message);
}

int thread_and_system(char cmd[], int argc, char *argv[]){
    int i;
    int needed = 1;                        // plus end of string mark

    needed += strlen(cmd);
    for(i = 1; i < argc; i++) {
        needed += strlen( argv[ i ] ) + 1; // plus space
    }

    // printf("%d\n", needed);

    char *storage = (char *) malloc( sizeof( char ) * needed );

    strcpy(storage, cmd);

    for( i = 1; i < argc; i++) {
        strcat( storage, " " );
        strcat( storage, argv[i]);
    }

    pthread_t our_thread;

    if (pthread_create(&our_thread, NULL, system_runner, (void *)storage)!=0){
        fprintf(stderr,"ERR! Could not create thread process\n");
        exit(EXIT_FAILURE);
    }

    // system(storage);
    pthread_join(our_thread, NULL);

    return 0;
}

int thread_commands(wordexp_t we){
    char *cmd_name = we.we_wordv[0];
    int argc = we.we_wordc;
    char **argv = we.we_wordv;


    if (strcmp(cmd_name, "ls") == 0){
        return thread_and_system("/bin/ls", argc, argv);
    }

    else if (strcmp(cmd_name, "cat") == 0){
        return thread_and_system("/bin/cat", argc, argv);
    }

    else if (strcmp(cmd_name, "mkdir") == 0){
        return thread_and_system("/bin/mkdir", argc, argv);
    }

    else if (strcmp(cmd_name, "rm") == 0){
        return thread_and_system("/bin/rm", argc, argv);
    }

    else if (strcmp(cmd_name, "date") == 0){
        return thread_and_system("/bin/date", argc, argv);
    }

    return -1;
}

int all_commands(wordexp_t we){
    char *cmd_name = we.we_wordv[0];
    // char *ifthread = we.we_wordv[sizeof(we.we_wordv)/sizeof(char *) - 1];
    int argc = we.we_wordc;
    char **argv = we.we_wordv;

    // all internal commands

    // printf("%s\n", we.we_wordv);

    if (strcmp(cmd_name, "cd") == 0){
        return cd(argc - 1, &argv[1]);
    }

    else if (strcmp(cmd_name, "echo") == 0){
        return echo(argc - 1, &argv[1]);
    }

    else if (strcmp(cmd_name, "pwd") == 0){
        return pwd(argc - 1, &argv[1]);
    }

    // all external commands 

    if (strcmp(cmd_name, "ls") == 0){
        return fork_and_exec("/bin/ls", argc, argv);
    }

    else if (strcmp(cmd_name, "cat") == 0){
        return fork_and_exec("/bin/cat", argc, argv);
    }

    else if (strcmp(cmd_name, "mkdir") == 0){
        return fork_and_exec("/bin/mkdir", argc, argv);
    }

    else if (strcmp(cmd_name, "rm") == 0){
        return fork_and_exec("/bin/rm", argc, argv);
    }

    else if (strcmp(cmd_name, "date") == 0){
        return fork_and_exec("/bin/date", argc, argv);
    }

    // All extra command

    if (strcmp(cmd_name, "exit") == 0){
        return exit_cmd(argc - 1, &argv[1]);
    }

    else if (strcmp(cmd_name, "help") == 0){
        return print_welcome_note();
    }

    return -1;
}


FILE *open_history(char *mode){
    char history_file[MAX_INP_SIZE];
    strcpy(history_file, working_path.start);
    strcat(history_file, HISTORY_FILE);
    FILE *fd = fopen(history_file, mode);
    if (fd == NULL){
        perror("history");
        exit(1);
    }
    return fd;
}

void add_to_history(char *cmd)
{
  FILE *fd = open_history("a");
  fprintf(fd, "%s%s", ftell(fd) ? "\n" : "", cmd);
  fclose(fd);
}

int main(int argc, char *argv[]){
    
    getcwd(working_path.current, MAX_INP_SIZE);
    strcpy(working_path.previous, working_path.current);
    strcpy(working_path.start, working_path.current);

    print_welcome_note();

    int ERR = 0;
    while(1)
    {
        printf("\npikachu ❯  ");
        char command[MAX_INP_SIZE];
        
        fgets(command, MAX_INP_SIZE, stdin);
        // printf("%s\n", command);
        command[strlen(command) - 1] = '\0';
        // printf("%s\n", command);
        if(command[0] == '\0' || command[0] == '\n')
            continue;

        add_to_history(command);
        int will_thread = 0;

        for (int stx=0; stx<strlen(command)-1; stx++){
            if (command[stx]=='&' && command[stx+1]=='t'){
                will_thread  = 1;
                command[stx] = '\0';
                command[stx+1] = '\0';
            }

            // printf("%c\n", command[stx]);
        }

        // printf("%s\n", command);
        
        wordexp_t we;
        int we_ret = wordexp(command, &we, 0);

        if (we_ret || !we.we_wordc)
        {
            wordfree(&we);
            continue;
        }
        if (will_thread==0){
            if ((ERR = all_commands(we)) == -1){
                fprintf(stderr, "pikachu: %s: command not found\n%s", we.we_wordv[0]);
            }
        }
        if (will_thread==1){
            if ((ERR = thread_commands(we)) == -1){
                fprintf(stderr, "pikachu: %s: command not found for thread\n%s", we.we_wordv[0]);
            }
        }

        wordfree(&we);
        
    }


    return 0;
}