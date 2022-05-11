#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "stdlib.h"

#include "limits.h"
#include "LineParser.h"

#define STDIN 0
#define STDOUT 1
#define MAX_PATH 4092
#define MAX_INPUT 2048


int execute(cmdLine *pCmdLine)
{
    int pid_t = fork();
    if (pid_t)
    {
        wait();
    } else
    {
        execv(pCmdLine->arguments[0], pCmdLine->arguments);
    }
}

int main(int argc, char **argv)
{
    char *cwd;
    getcwd(cwd, MAX_PATH);
    write(STDOUT, cwd, strlen(cwd));
    char *user_input;
    fgets(user_input, MAX_INPUT, (FILE *) 1);
    cmdLine *current_cmd = parseCmdLines(user_input);

}