#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#define BOOL int
#define TRUE 1
#define FALSE 0
#define ARG_MAX 256
#define FILE_NAME_MAX 128
#define ARG_NUM 64
#define FD_UNDEF -1

void sh_mainloop();

void sh_pipe(char *command);

void sh_dup(char *command, int pipe_read, int pipe_write, BOOL need_wait);

void find_filename(char **dst, char *src);

int split(char *command, int *argc, char *argv[]);

void mysys(char *command, int fd_read, int fd_write, BOOL need_wait);

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
        int status = chdir(argv);
        if (status != 0)
            printf("cd: no such file or directory: %s\n", argv);
    }
    else if (strstr(p, "pwd") == p)
        printf("%s\n", getcwd(NULL, 0));
    else if (strstr(p, "exit") == p)
        exit(0);
    else if (strlen(command) > 0)
    {
        sh_pipe(command);
    }
    return;
}

void sh_pipe(char *command)
{
    int fd_pipe[2];
    int len = strlen(command);
    int i = 0, status;
    char *p;
    char *cmds[ARG_NUM];
    int pipe_c = 0;
    cmds[pipe_c++] = command;
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
    if (pipe_c == 1)
    {
        // TODO: single command
        sh_dup(cmds[0], FD_UNDEF, FD_UNDEF, TRUE);
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
        {
            sh_dup(cmds[i], FD_UNDEF, fd_pipe[1], TRUE);
        }
        else if (i != pipe_c - 1)
        {
            // FIXME: read from pipe and write to pipe
            sh_dup(cmds[i], fd_pipe[0], fd_pipe[1], TRUE);
        }
        else
        {
            sh_dup(cmds[i], fd_pipe[0], FD_UNDEF, TRUE);
        }
    }
}

void sh_dup(char *command, int pipe_read, int pipe_write, BOOL need_wait)
{
    int i, len = strlen(command), t;
    BOOL f_stdout = FALSE, f_stdin = FALSE;
    char *file_input, *file_output;
    int fd_read = FD_UNDEF, fd_write = FD_UNDEF;
    for (i = 0; i < len; ++i)
    {
        if (command[i] == '>')
        {
            command[i] = '\0';
            find_filename(&file_output, command + i + 1);
            f_stdout = TRUE;
        }
        else if (command[i] == '<')
        {
            command[i] = '\0';
            find_filename(&file_input, command + i + 1);
            f_stdin = TRUE;
        }
    }
    if (f_stdin)
    {
        fd_read = open(file_input, O_CREAT | O_RDWR, 0666);
    }
    else if (pipe_read > FD_UNDEF)
    {
        fd_read = pipe_read;
    }
    else
    {
        fd_read = FD_UNDEF;
    }

    if (f_stdout)
    {
        fd_write = open(file_output, O_CREAT | O_RDWR, 0666);
    }
    else if (pipe_write > FD_UNDEF)
    {
        fd_write = pipe_write;
    }
    else
    {
        fd_write = FD_UNDEF;
    }
    mysys(command, fd_read, fd_write, need_wait);
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

void mysys(char *command, int fd_read, int fd_write, BOOL need_wait)
{
    pid_t pid;
    char *p = NULL;
    int status;
    char *argv[ARG_NUM];
    int argc;
    split(command, &argc, argv);
    argv[argc] = NULL;

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
        if (need_wait)
        {
            waitpid(pid, NULL, 0);
        }
        return;
    }
    else
    {
        if (fd_read != FD_UNDEF)
        {
            close(STDIN_FILENO);
            status = dup2(fd_read, STDIN_FILENO);
            close(fd_read);
            if (status < 0)
            {
                printf("%s dup stdin failed: %s\n", argv[0], strerror(errno));
            }
        }
        if (fd_write != FD_UNDEF)
        {
            close(STDOUT_FILENO);
            status = dup2(fd_write, STDOUT_FILENO);
            close(fd_write);
            if (status < 0)
            {
                printf("%s dup stdout failed: %s\n", argv[0], strerror(errno));
            }
        }
        if (execvp(argv[0], argv) == -1)
        {
            printf("%s exec failed: %s\n", argv[0], strerror(errno));
        }
        exit(0);
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