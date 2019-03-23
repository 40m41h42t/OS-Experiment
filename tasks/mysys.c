#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
// TODO: rewrite through strtok
void mysys(char *command)
{
    //pid = fork();
    pid_t pid;
    int status;
    if (command == NULL)
    {
        printf("err command\n");
        return;
    }
    pid = fork();
    if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", command, NULL);
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