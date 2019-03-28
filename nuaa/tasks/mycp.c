#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFMAXSIZE 1024

int main(int argc, char **argv)
{
    char *rdpathname = argv[1];
    char *wrpathname = argv[2];
    char buf[1024];
    int in, out;
    int flag;
    in = open(rdpathname, O_RDONLY);
    if (in == -1)
    {
        return -1;
    }
    out = open(wrpathname, O_CREAT | O_WRONLY);
    if (out == -1)
    {
        return -1;
    }
    while ((flag = read(in, buf, BUFMAXSIZE)) > 0)
    {
        write(out, buf, flag);
    }
    close(in);
    close(out);
    return 0;
}