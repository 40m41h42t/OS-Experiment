#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
// #include <sys/types.h>
#include <sys/wait.h>
// #define BOOL int
#define BUFFER_SIZE 0x1000
int main(int argc, char *argv[])
{
    // usage:
    // in su: ./task1 <number>
    // in other users: sudo ./task1 <number>
    pid_t pid;
    int i, num, shmID;
    char *shmAddr = NULL;
    int *fibNum = NULL;
    // check parameter
    if (argc < 2)
    {
        printf("please add parameters!\nusage: ./task1 <number>\n");
        exit(0);
    }
    for (i = 0; i < strlen(argv[1]); ++i)
    {
        if (!(argv[1][i] >= '0' && argv[1][i] <= '9'))
        {
            printf("please input right number!\nusage: ./task1 <number>\n");
            exit(1);
        }
    }
    num = atoi(argv[1]);
    // create shared memory
    if ((shmID = shmget(IPC_PRIVATE, (num + 1) * sizeof(int), IPC_CREAT | IPC_EXCL)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    { // in child process
        if ((shmAddr = (char *)shmat(shmID, 0, 0)) == (char *)-1)
        {
            perror("Child: shmat");
            exit(1);
        }
        fibNum = (int *)shmAddr;
        fibNum[0] = 0;
        if (num >= 1)
        {
            fibNum[1] = 1;
            for (i = 2; i < num; ++i)
                fibNum[i] = fibNum[i - 1] + fibNum[i - 2];
        }
        if (shmdt(shmAddr) == -1)
        {
            perror("Child: shmdt");
        }
    }
    else
    { // in parent process
        waitpid(pid, NULL, 0);
        if ((shmAddr = (char *)shmat(shmID, 0, 0)) == (char *)-1)
        {
            perror("Parent: shmat");
            exit(1);
        }
        fibNum = (int *)shmAddr;
        for (i = 0; i < num; ++i)

            printf("%d\n", fibNum[i]);
        if (shmctl(shmID, IPC_RMID, NULL) == -1)
        {
            perror("Parent: shmctl");
            exit(1);
        }
    }
    return 0;
}