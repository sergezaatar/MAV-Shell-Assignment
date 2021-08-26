/*
	Name: Serge Zaatar Antor
	ID: 1001623346
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"
/* We want to split our command line up into tokens \
so we need to define what delimits our tokens.   \
In this case  white space                        \
will separate the tokens on our command line
*/

#define MAX_COMMAND_SIZE 255 // The maximum command-line size
#define HISTORY_MAX 15
#define MAX_NUM_ARGUMENTS 11 // Mav shell only supports eleven arguments

int main()
{

    char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);
    char *test_t = (char *)malloc(MAX_COMMAND_SIZE);
    char *history[HISTORY_MAX];
    int hisCounter = 0, hisIndex = 0, pidCounter = 0;
    int i = 0, full = 0, pidfull = 0, indexxx;
    int pidsArray[15];

    // Makes the history array
    for (i = 0; i < 15; i++)
    {
        history[i] = (char *)malloc(MAX_COMMAND_SIZE);
        memset(history[i], 0, MAX_COMMAND_SIZE);
    }

    while (1)
    {

        // Print out the msh prompt
        printf("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
            ;


        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int token_count = 0;

        // Pointer to point to the token
        // parsed by strsep
        char *arg_ptr;
        
        // If the user entered !, take the number after the ! as a string and convert it to integer. Then use that number as an index for the history and copy it to the cmd_str array
        // Which will then work as if we entered the command normally and go through all the if statements.
        if (cmd_str[0] == '!' )
        {
            test_t = &cmd_str[1];
            indexxx = atoi(test_t) - 1;
            if (indexxx > 14 || indexxx < 0 )
            {
                printf("Error. Out of command size max (Number should be between 1 and 15)\n");
                continue;
            }
            memset(cmd_str, 0, MAX_COMMAND_SIZE);
            strncpy(cmd_str, history[indexxx], MAX_COMMAND_SIZE);
        }
        
        char *working_str = strdup(cmd_str);

        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;

        // Tokenize the input stringswith whitespace used as the delimiter
        while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
               (token_count < MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
            if (strlen(token[token_count]) == 0)
            {
                token[token_count] = NULL;
            }
            token_count++;
        }


        //Checking for a blank line, prompting again each time.
        // It is put before the strncpy so it does not segfaults when a blank is entered

        if (!strcmp(cmd_str, "\n"))
        {
            continue;
        }

        // Copies the command string into the history array
        // Each time the user enters a command, it is added to the history array
        strncpy(history[hisCounter++], cmd_str, strlen(cmd_str));

        // If the user entered 15 or more commands, we make a signal with the full variable
        // so we know that we reached the limit of the history array and the commands will start to shift

        if (hisCounter > 14)
        {
            hisCounter = 0;
            full = 1;
        }

        if (pidCounter > 14)
        {
            pidCounter = 0;
            pidfull = 1;
        }

        // Whenever user enters commands exit or quit, program exits
        if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit"))
        {
            exit(0);
        }

        // When user enters command cd we user chdir function to change directories. 
        // This takes all the parameters of token
        // By using token[1]
        if (!strcmp(token[0], "cd"))
        {
            chdir(token[1]);
            continue;
        }

        // When user enters command history, a list of the last 15 (or less) commands inputted by the user is going appear
        if (!strcmp(token[0], "history"))
        {
            // If we have not gone past 15 commands we enter this if statement
            if (!full)
            {
                for (hisIndex = 0; hisIndex < hisCounter; hisIndex++)
                {
                    printf("%d. %s", hisIndex + 1, history[hisIndex]);
                }
            }

            // If we have gone past 15 commands, the full variable will be equal to 1, and we will enter this statement
            else
            {
                int newIndex = hisCounter;
                for (hisIndex = 0; hisIndex < 15; hisIndex++)
                {
                    printf("%d. %s", hisIndex + 1, history[newIndex++]);
                    if (newIndex > 14)
                        newIndex = 0;
                }
            }
            continue;
        }
        
        // When the user enters command showpids we enter this statement. It will show the pids for each command entered
        if (!strcmp(token[0], "showpids"))
        {
            if (!pidfull)
            {
                for (i = 0; i < pidCounter; i++)
                {
                    printf("%d. %d\n", i + 1, pidsArray[i]);
                }
            }

            // If we have gone past 15 commands, the full variable will be equal to 1, and we will enter this statement
            else
            {
                int newPidIndex = pidCounter;
                for (i = 0; i < 15; i++)
                {
                    printf("%d. %d\n", i + 1, pidsArray[newPidIndex++]);
                    if (newPidIndex > 14)
                        newPidIndex = 0;
                }
            }
            continue;
        }


        // forks to create a child
        pid_t pid = fork();

        if (pid == -1)
        {
            printf("fork failed\n");
            exit(0);
        }
        // If child
        else if (pid == 0)
        {
            // Using execvp to take the commands from /bin/, /usr/bin/, /usr/local/bin 
            // and the current working directory
            int ret = execvp(token[0], token);

            if (ret == -1)
            {
                printf("%s: Command not found.\n", token[0]);
                //perror("exec failed: ");
            }

            // return 0 after execvp so if it fails we don't forkbomb
            return 0;
        }
        else
        {
            // Adds the pid values inside the array of pids
            if (pidCounter < 15)
            {
                pidsArray[pidCounter] = pid;
                pidCounter++;
            }
            else
            {
                pidCounter = 0;
            }

            int status;
            wait(&status);
        }

        free(working_root);
    }

    return 0;
}
