#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
char buf[1024];
void split(char *command, int *argc, char *argv[])
{
    strncpy(buf, command, strlen(command));
    char *str = buf;
    char spi[] = " ";
    char *p = NULL;
    *argc = 0;
    argv[*argc] = strtok(str, spi);
    ++*argc;
    while ((p = strtok(NULL, spi)))
    {
        argv[*argc] = p;
        ++*argc;
    }
}

void mysys(char *command)
{
    pid_t pid;
    int status;
    int wordc;
    char *wordn[8];
    if (command == NULL)
    {
        printf("err command\n");
        return;
    }
    split(command, &wordc, wordn);
    wordn[wordc] = NULL;

    pid = fork();
    if (pid == 0)
    {
        // execl("/bin/sh", "sh", "-c", command, NULL);
        int pcode = execvp(wordn[0], wordn);
        if (pcode < 0)
        {
            perror("execvp");
        }
        exit(123);
    }
    else
    {
        wait(&status);
    }
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}