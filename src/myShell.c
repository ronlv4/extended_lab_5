#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "limits.h"
#include "LineParser.h"

#define STDIN 0
#define STDOUT 1
#define MAX_PATH 4092
#define MAX_INPUT 2048


int execute(cmdLine *pCmdLine)
{
    int status;
    int pid_t = fork();
    if (pid_t)
    {
        wait(&status);
        return status;

    } else
    {
        execv(pCmdLine->arguments[0], pCmdLine->arguments);
        perror(NULL);
    }
}

int main(int argc, char **argv)
{
    char *cwd;
    cmdLine *current_cmd;
    getcwd(cwd, MAX_PATH);
    write(STDOUT, cwd, strlen(cwd));
    char *user_input;
    fgets(user_input, MAX_INPUT, (FILE *) 1);
    while (strcmp(user_input, "quit"))
    {
        current_cmd = parseCmdLines(user_input);
        execute(current_cmd);
    }
    return 0;
}