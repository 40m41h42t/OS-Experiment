#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#define BOOL int
#define TRUE 1
#define FALSE 0
#define ARG_MAX 256
#define ARG_NUM 64
#define FD_UNDEF -1

void sh_mainloop();

void sh_pipe(char *command);

void sh_dup(char *command, int pipe_read, int pipe_write);

void find_filename(char **dst, char *src);

int split(char *command, int *argc, char *argv[]);

void mysys(char *command, int fd_read, int fd_write);

int main()
{
    while (TRUE)
        sh_mainloop();
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
        ++p;

    // p points at first of command
    if (strstr(p, "cd") == p)
    {
        sscanf(p, "cd %s", argv);
        int status = chdir(argv);
        if (status != 0)
            printf("cd: no such file or directory: %s\n", argv);
    }
    else if (strstr(p, "pwd") == p)
        printf("%s\n", getcwd(NULL, 0));
    else if (strstr(p, "exit") == p)
        exit(0);
    else if (strlen(p) > 0)
        sh_pipe(p);
    return;
}

void sh_pipe(char *command)
{
    int fd_pipe[2], fd_tmp;
    int len = strlen(command);
    int i = 0, status, pipe_c = 0;
    char *p, *cmds[ARG_NUM];
    cmds[pipe_c++] = command;
    for (i = 0; i < len; ++i)
    {
        if (command[i] == '|')
        {
            command[i] = '\0';
            // left, input pipe
            p = command + i - 1;
            while (*p == ' ' || *p == '\n')
            {
                *p = '\0';
                --p;
            }
            // right, output pipe
            p = command + i + 1;
            while (*p && (*p == ' ' || *p == '\n'))
            {
                *p = '\0';
                ++p;
            }
            cmds[pipe_c++] = p;
        }
    }
    if (pipe_c == 1)
    {
        // no pipe
        sh_dup(cmds[0], FD_UNDEF, FD_UNDEF);
        return;
    }

    status = pipe(fd_pipe);
    if (status == -1)
    {
        printf("can't open pipe\n");
        return;
    }

    for (i = 0; i < pipe_c; ++i)
    {
        if (i == 0)
            // input to pipe
            sh_dup(cmds[i], FD_UNDEF, fd_pipe[1]);
        else if (i != pipe_c - 1)
        {
            fd_tmp = fd_pipe[0];
            status = pipe(fd_pipe);
            if (fd_tmp == -1)
            {
                printf("can't open pipe.\n");
                return;
            }
            sh_dup(cmds[i], fd_tmp, fd_pipe[1]);
        }

        else
            // output from pipe
            sh_dup(cmds[i], fd_pipe[0], FD_UNDEF);
    }
}

void sh_dup(char *command, int pipe_read, int pipe_write)
{
    int fd_read = FD_UNDEF, fd_write = FD_UNDEF;
    int i, len = strlen(command);
    BOOL f_stdout = FALSE, f_stdin = FALSE;
    char *file_input, *file_output;
    char *p;
    for (i = 0; i < len; ++i)
    {
        if (command[i] == '>')
        {
            p = command + i - 1;
            while (*p == ' ')
                *(p--) = '\0';
            command[i] = '\0';
            find_filename(&file_output, command + i + 1);
            f_stdout = TRUE;
        }
        else if (command[i] == '<')
        {
            find_filename(&file_input, command + i + 1);
            p = command + i - 1;
            while (*p == ' ')
                *(p--) = '\0';
            f_stdin = TRUE;
        }
    }
    if (f_stdin)
        fd_read = open(file_input, O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
    else if (pipe_read > FD_UNDEF)
        fd_read = pipe_read;
    else
        fd_read = FD_UNDEF;

    if (f_stdout)
        fd_write = open(file_output, O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
    else if (pipe_write > FD_UNDEF)
        fd_write = pipe_write;
    else
        fd_write = FD_UNDEF;
    mysys(command, fd_read, fd_write);
}

void find_filename(char **dst, char *src)
{
    int i;
    while (*src == ' ')
        ++src;
    *dst = src;
    for (i = 0; i < strlen(src); ++i)
        if (src[i] == ' ' || src[i] == '\n')
            src[i] = '\0';
}

void mysys(char *command, int fd_read, int fd_write)
{
    pid_t pid;
    int status, argc;
    char *p = NULL, *argv[ARG_NUM];

    pid = fork();
    if (pid == -1)
    {
        printf("fork failed\n");
        exit(-1);
    }
    else if (pid != 0)
    {
        if (fd_read != FD_UNDEF)
            close(fd_read);
        if (fd_write != FD_UNDEF)
            close(fd_write);
        waitpid(pid, NULL, 0);
    }
    else
    {
        if (fd_read != FD_UNDEF)
        {
            close(STDIN_FILENO);
            status = dup2(fd_read, STDIN_FILENO);
            close(fd_read);
            if (status < 0)
                printf("%s dup stdin failed: %s\n", argv[0], strerror(errno));
        }
        if (fd_write != FD_UNDEF)
        {
            close(STDOUT_FILENO);
            status = dup2(fd_write, STDOUT_FILENO);
            close(fd_write);
            if (status < 0)
                printf("%s dup stdout failed: %s\n", argv[0], strerror(errno));
        }
        split(command, &argc, argv);
        argv[argc] = NULL;
        if (execvp(argv[0], argv) == -1)
            printf("%s exec failed: %s\n", argv[0], strerror(errno));
        exit(0);
    }
}

int split(char *command, int *argc, char *argv[])
{
    char *p = NULL;
    int i;
    *argc = 0;
    argv[(*argc)++] = command;
    for (i = 0; i < strlen(command); ++i)
    {
        if (command[i] == ' ')
        {
            command[i] = '\0';
            p = command + i + 1;
            while (*p && (*p == ' '))
            {
                ++p;
            }
            argv[(*argc)++] = p;
        }
    }
    return 0;
}