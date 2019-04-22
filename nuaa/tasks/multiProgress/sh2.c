#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BOOL int
#define TRUE 1
#define FALSE 0
#define ARG_MAX 1024
#define FILE_NAME_MAX 128

static char cr_lf[] = "\n";

int split(char *command, int *argc, char *argv[]);

int mysys(char *command);

int my_cd(char *args[]);

int my_pwd(char *args[]);

int my_exit(char *args[]);

static struct
{
    char *name;
    int (*handler)(char **);
} built_in_cmd_table[] = {
    {"cd", my_cd},
    {"pwd", my_pwd},
    {"exit", my_exit},
};

#define NR_BUILTIN_CMD (sizeof(built_in_cmd_table) / sizeof(built_in_cmd_table[0]))

int main()
{
    char bash_basic_output[] = "> ";
    char command[ARG_MAX + 1];
    while (TRUE)
    {
        memset(command, 0, (ARG_MAX + 1) * sizeof(char));
        write(STDOUT_FILENO, bash_basic_output, sizeof(bash_basic_output) / sizeof(char) - 1);
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
    // in-bash command won't fork new process
    for (i = 0; i < NR_BUILTIN_CMD; i++)
        if (strcmp(argv[0], built_in_cmd_table[i].name) == 0)
            if (built_in_cmd_table[i].handler(argv) == TRUE)
                return 0;
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

int my_cd(char *args[])
{
    static char err_output[] = "cd: no such file or directory: ";
    int errno = chdir(args[1]);
    if (errno != 0)
    {
        write(STDOUT_FILENO, err_output, sizeof(err_output) / sizeof(char) - 1);
        write(STDOUT_FILENO, args[1], sizeof(args[1]) / sizeof(char) - 1);
        write(STDOUT_FILENO, cr_lf, sizeof(cr_lf) / sizeof(char) - 1);
    }
    return TRUE;
}

int my_pwd(char *args[])
{
    char *buf = getcwd(NULL, 0);
    write(STDOUT_FILENO, buf, strlen(buf));
    write(STDOUT_FILENO, cr_lf, sizeof(cr_lf) / sizeof(char) - 1);
    return TRUE;
}

int my_exit(char *args[])
{
    exit(0);
    return TRUE;
}