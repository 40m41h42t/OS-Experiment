#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0
#define ARG_MAX 1024
#define ARG_ZERO 17

int split(char *command, int *argc, char *argv[]);
int mysys(char *command);

int main()
{
    char command[ARG_MAX + 1];
    while (TRUE)
    {
        memset(command, 0, (ARG_MAX + 1) * sizeof(char));
        printf("> ");
        fgets(command, ARG_MAX, stdin);
        setbuf(stdin, NULL);
        // if get command through fgets,
        // the last of command contains '\n'
        if (command[strlen(command) - 1] == '\n')
            command[strlen(command) - 1] = '\0';
        mysys(command);
    }
}

int split(char *command, int *argc, char *argv[])
{
    char buf[ARG_MAX + 1];
    memset(buf, 0, sizeof(char) * (ARG_MAX + 1));
    strncpy(buf, command, strlen(command));
    char *str = buf;
    char split_str[] = " ";
    char *p = NULL;
    *argc = 0;
    argv[*argc] = strtok(str, split_str);
    ++*argc;
    while ((p = strtok(NULL, split_str)))
    {
        argv[*argc] = p;
        ++*argc;
    }
    return 0;
}
int mysys(char *command)
{
    pid_t pid;
    int status;
    if (command == NULL)
    {
        return 0;
    }
    // split input command

    char *argv[100];
    int argc;
    split(command, &argc, argv);
    argv[argc] = NULL;
    // in-bash command won't fork new process
    int flag = FALSE;
    if (strcmp(argv[0], "cd") == 0)
    {
        int errno = chdir(argv[1]);
        if (errno == -1)
            printf("cd: no such file or directory: %s\n", argv[1]);
        flag = TRUE;
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        printf("%s\n", getcwd(NULL, 0));
        flag = TRUE;
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
        exit(0);
    }
    if (flag == TRUE)
        return 0;
    // while command isn't NULL,
    // fork a new process to execute
    pid = fork();
    if (pid == 0)
    {

        if (execvp(argv[0], argv) < 0)
            perror("execvp");
        exit(-1);
    }
    else
    {
        wait(&status);
    }
    return 0;
}