#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAXN 10000000
#define NR_CPU 16

struct param
{
    int start, end;
};
struct Sresult
{
    double result;
};
void *Slave_Thread(void *arg)
{
    int i = ((struct param *)arg)->start;
    struct Sresult *ret = (struct Sresult *)malloc(sizeof(struct Sresult));
    ret->result = 0;
    if (!(i & 1))
        i += 1;
    for (i; i <= ((struct param *)arg)->end; i += 2)
        ret->result += (i % 4 == 1 ? 1 : -1) * (double)1 / i;

    return (void *)ret;
}

int main()
{
    pthread_t workers[NR_CPU];
    struct param params[NR_CPU];
    int j;
    for (j = 0; j < NR_CPU; ++j)
    {
        // printf("main i: %d\n", i);
        struct param *slavepara;
        slavepara = &params[j];
        slavepara->start = ((j == 0) ? 1 : (MAXN * j / NR_CPU));
        slavepara->end = MAXN * (j + 1) / NR_CPU;
        pthread_create(&workers[j], NULL, Slave_Thread, slavepara);
    }
    double pi = 0;
    for (j = 0; j < NR_CPU; ++j)
    {
        struct Sresult *result;
        pthread_join(workers[j], (void **)&result);
        pi += result->result;
    }
    printf("%lf\n", pi * 4);
    return 0;
}