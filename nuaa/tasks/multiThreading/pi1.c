#include <stdio.h>
#include <pthread.h>
#define MAXN 10000000
double SlaveSum = 0;
double MasterSum = 0;
void *Slave_Thread(void *arg)
{
    int i = MAXN >> 1;
    if (!(i & 1))
        i += 1;
    for (i; i <= MAXN; i += 2)
        SlaveSum += (i % 4 == 1 ? 1 : -1) * (double)1 / i;
}

void Master_Thread()
{
    int i;
    for (i = 1; i<MAXN>> 1; i += 2)
        MasterSum += (i % 4 == 1 ? 1 : -1) * (double)1 / i;
}
int main()
{
    pthread_t workTid;
    double sum = 0;
    pthread_create(&workTid, NULL, Slave_Thread, NULL);
    Master_Thread();
    pthread_join(workTid, NULL);
    sum = MasterSum + SlaveSum;
    printf("%lf\n", sum*4);
}