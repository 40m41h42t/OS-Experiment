#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BOOL int
#define TRUE 1
#define FALSE 0
#define ARG_MAX 256
#define FILE_NAME_MAX 128
#define PIPE_NUM 64

void sh_mainloop();

void sh_pipe(char *command);

int split(char *command, int *argc, char *argv[]);

int mysys(char *command);

int my_cd(char *args);

int my_pwd(char *args);

int my_exit(char *args);

int main()
{

    while (TRUE)
    {
        sh_mainloop();
    }
}

void sh_mainloop()
{
    /* sh_mainloop function, which contains build-in bash
    */
    int i;
    char *p;
    char command[ARG_MAX + 1], argv[ARG_MAX + 1];

    memset(command, 0, (ARG_MAX + 1) * sizeof(char));
    printf("\n# qrzbing @ QRZ in %s\n$ ", getcwd(NULL, 0));
    fgets(command, ARG_MAX, stdin);

    if (command == NULL)
        return;

    p = command + strlen(command) - 1;
    while (*p == ' ' || *p == '\n')
    {
        *p = '\0';
        --p;
    }
    p = command;
    while (*p && (*p == ' ' || *p == '\n'))
    {
        ++p;
    }
    sh_pipe(p);
    // p points at first of command
    if (strstr(p, "cd") == p)
    {
        sscanf(p, "cd %s", argv);
        int errno = chdir(argv);
        if (errno != 0)
            printf("cd: no such file or directory: %s\n", argv);
    }
    else if (strstr(p, "pwd") == p)
        printf("%s\n", getcwd(NULL, 0));
    else if (strstr(p, "exit") == p)
        exit(0);
    else if (strlen(command) > 0)
    {
        mysys(command);
        return;
    }
    return;
}

void sh_pipe(char *command)
{
    int fd_pipe[2];
    int len = strlen(command);
    int i = 0, status;
    char *p;
    char *cmds[PIPE_NUM];
    int pipe_c = 0;
    cmds[pipe_c] = command;
    for (i = 0; i < len; ++i)
    {
        if (command[i] == '|')
        {
            command[i] = '\0';
            p = command + i - 1;
            while (*p == ' ' || *p == '\n')
            {
                *p = 0;
                --p;
            }
            p = command + i + 1;
            while (*p && (*p == ' ' || *p == '\n'))
            {
                *p = 0;
                ++p;
                ++i;
            }
            cmds[pipe_c++] = p;
        }
    }
    if(pipe_c == 1){
        // TODO: single command
        return;
    }
    
    status = pipe(fd_pipe);
    if(status == -1){
        printf("can't open pipe\n");
        return;
    }

    for(i = 0;i < pipe_c; ++i){
        if(i == 0){
            // TODO: write_to_pipe
        }
        else if(i != pipe_c -1){
            // TODO: read from pipe and write to pipe
        }
        else{
            // TODO: write to pipe
        }
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
    // init variables

    // pid variables
    pid_t pid;
    int status;
    // command variables
    char *argv[100];
    int argc;
    // redirect variables
    int redir_loc;
    char redir_filename[FILE_NAME_MAX + 1];
    int fd;
    // temp variables
    int i;
    int j;

    if (command == NULL)
        return 0;

    // at first, check if need redirection
    BOOL redir_flag = FALSE;
    for (i = 0; i < strlen(command); i++)
        if (command[i] == '>')
        {
            redir_flag = TRUE;
            break;
        }
    if (redir_flag)
    {
        redir_loc = i;
        // memcpy from '>'+1 to end of command.
        // However, space is unuseful which need delete.
        for (i = i + 1, j = 0; i < strlen(command) && j < FILE_NAME_MAX; ++i)
            if (command[i] != ' ')
            {
                redir_filename[j] = command[i];
                ++j;
            }
        redir_filename[j] = '\0';
        command[redir_loc] = '\0';
    }
    // split input command
    split(command, &argc, argv);
    argv[argc] = NULL;

    // while command isn't NULL,
    // fork a new process to execute
    pid = fork();
    // stdin;
    if (pid == 0)
    {
        if (redir_flag)
        {
            fd = open(redir_filename, O_CREAT | O_RDWR, 0666);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (execvp(argv[0], argv) < 0)
            perror("execvp");
        exit(-1);
    }
    else
        wait(&status);
    return 0;
}
