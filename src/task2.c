#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "limits.h"
#include "string.h"
#include "LineParser.h"

#define STDIN 0
#define STDOUT 1
#define MAX_PATH 4092
#define MAX_LENGTH 2048

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process {
    cmdLine *cmd; /* the parsed command line*/
    pid_t pid; /* the process id that is
		  running the command*/
    int status; /* status of the process:
		   RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
} process;

void updateProcessList();
void nap();
void stop();
void updateProcessStatus();
void addProcess();
void printProcessList();
void freeProcessList();
void simulate_chdir();
int execute();


void nap(int time, int ps_pid)
{
    int status;
    int pid = fork();
    if (pid == -1)
    {
        perror(NULL);
        _exit(pid);
    }
    if (pid)
    {
        wait(&status);
    }
    else
    {
        kill(ps_pid, SIGTSTP);
        sleep(time);
        kill(ps_pid, SIGCONT);
    }
}

void stop(int pid)
{
    kill(pid, SIGINT);
}

void updateProcessList(process **process_list)
{
    int status, ans;
    process *cur = *process_list;
    while (cur)
    {
        pid_t ret = waitpid(cur->pid, &ans, WNOHANG | WCONTINUED | WUNTRACED);
        if(ret != 0){
            status =  (WIFSTOPPED(ans)) ? SUSPENDED :
                (WIFSIGNALED(ans)|| WIFEXITED(ans)) ? TERMINATED :
                RUNNING;
            updateProcessStatus(cur, cur->pid, status);
        }
        cur = cur->next;
    }
}

void updateProcessStatus(process *process_list, int pid, int status)
{
    process *current_process = process_list;
    
    while (current_process)
    {
        if (current_process->pid != pid)
        {
            current_process = current_process->next;
            continue;
        }
        current_process->status = status;
        return;
    }
    printf("no such process - %d\n", pid);
}

void addProcess(process **process_list, cmdLine *cmd, pid_t pid) {
    process *ps = (process *) malloc(sizeof(process));
    ps->cmd = cmd;
    ps->pid = pid;
    ps->status = RUNNING;
    if (process_list == NULL)
    {
        process_list = malloc(sizeof(process*));
        process_list = &ps;
        return;
    }
    ps->next = *process_list;
    *process_list = ps;
}

void printProcessList(process **process_list) {
    if (*process_list == NULL) {
        printf("process list is empty\n");
        return;
    }
    updateProcessList(process_list);

    printf("process_id\t\tcommand\t\tprocess_status\n");
    process *current_process = *process_list;
    process *prev = *process_list;

    while (current_process)
    {
        char *string_status = current_process->status == -1 ? "TERMINATED":
                                current_process-> status == 0 ? "RUNNING" :
                                "SUSPENDED";
        printf("%d\t\t\t%s\t\t%s\n", current_process->pid, current_process->cmd->arguments[0], string_status);
        if (current_process->status == TERMINATED)
        {
            if (current_process == *process_list)
            {
                *process_list = NULL;
                return;
            }            
            prev->next = current_process->next;
            free(current_process);
            current_process = prev;
        }
        prev = current_process;
        current_process = current_process->next;
    }
}

void freeProcessList(process *process_list) {
    process *current;
    process *next;
    if (process_list == NULL)
        return;
    current = next = process_list;
    while (current != NULL) {
        next = next->next;
        free(current);
        current = next;
    }
}


void simulate_chdir(char *cwd, char *path) {
    strcat(cwd, "/");
    strcat(cwd, path);
    if (chdir(cwd) == -1)
        perror(NULL);
}


int execute(cmdLine *pCmdLine, process **process_list) {
    int status;
    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        perror(NULL);
        _exit(1);
    }

    if (ch_pid > 0) {
        if (pCmdLine->blocking)
            wait(&status);
        addProcess(process_list,pCmdLine,ch_pid);
    } else {
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
        {
            perror(NULL);
            _exit(errno);
        }      
    }
    return 0;
}

int main(int argc, char **argv) {
    char cwd[MAX_PATH];
    process *process_list = NULL;
    cmdLine *current_cmd;
    char user_input[MAX_LENGTH];
    while (1) {
        getcwd(cwd, MAX_PATH);
        write(STDOUT, cwd, strlen(cwd));
        write(STDOUT, "$ ", 2);
        fflush(stdout);
        fgets(user_input, MAX_INPUT, stdin);
        fflush(stdout);
        user_input[strlen(user_input) - 1] = '\0';
        if (strcmp(user_input, "quit") == 0)
            _exit(0);
        if (strcmp(user_input, "") == 0)
            continue;
        current_cmd = parseCmdLines(user_input);
        if (strcmp(current_cmd->arguments[0], "cd") == 0) {
            simulate_chdir(cwd, current_cmd->arguments[1]);
            continue;
        }
        if (strcmp(current_cmd->arguments[0], "showprocs") == 0) {
            printProcessList(&process_list);
            continue;
        }
	if (strcmp(current_cmd->arguments[0], "nap") == 0)
	{
	    nap(atoi(current_cmd->arguments[1]), atoi(current_cmd->arguments[2]));
	    continue;
	}
        execute(current_cmd, &process_list);
    }
    return 0;
}
