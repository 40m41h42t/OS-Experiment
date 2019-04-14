#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define BUFMAXSIZE 1024

int main(int argc, char **argv)
{
    char *pathname = argv[1];
    char buf[1024];
    int fp = open(pathname, O_RDONLY);
    int rdnum;
    if (fp == -1)
    {
        return -1;
    }
    while ((rdnum = read(fp, buf, BUFMAXSIZE)) > 0)
    {
        write(STDOUT_FILENO, buf, rdnum);
    }
    close(fp);
    return 0;
}