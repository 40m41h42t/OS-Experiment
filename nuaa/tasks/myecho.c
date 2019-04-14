#include <stdio.h>
int main(int argc, char **argv)
{
    // printf("%d\n", argc);
    int i = 1;
    for (; i < argc; ++i)
    {
        printf("%s ", argv[i]);
    }
    putchar('\n');
    return 0;
}