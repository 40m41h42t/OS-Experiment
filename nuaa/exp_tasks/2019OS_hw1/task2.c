#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define MAX_PRIME 0xFFFF
#define True 1
#define False 0
#define Bool int
#define Uint32_t unsigned int
Bool Prime_0xFFFF[MAX_PRIME] = {False};
static Uint32_t Prime[0xFFFFFF];
static int SlaveNum = 0;
static Uint32_t SlavePrime[0xFFFFF];
static int primeNum = 0;
static int basicNum = 0;
static Uint32_t maxn;
Bool isPrime(Uint32_t num)
{
    for (int i = 0; i < MAX_PRIME + 1; ++i)
        if (Prime[i] * Prime[i] <= num)
        {
            if (num % Prime[i] == 0)
                return False;
            else
                continue;
        }
        else
            break;
    return True;
}
void checkPrime(Uint32_t maxn)
{
    Prime_0xFFFF[0] = True;
    Prime_0xFFFF[1] = True;
    Uint32_t i, j;
    for (i = 2; i < maxn; ++i)
        if (Prime_0xFFFF[i] == False)
        {
            Prime[primeNum++] = i;
            for (j = i * i; j < maxn; j += i)
                Prime_0xFFFF[j] = True;
        }
}
void *Slave_Thread(void *arg)
{
    Uint32_t mid = (Uint32_t)(0xFFFF + maxn) / 2;
    if(!(mid&0x1)) mid-=1;
    for (Uint32_t i = mid; i <= maxn; i += 2)
        if (isPrime(i) == True)
            SlavePrime[SlaveNum++] = i;
}
void Master_Thread()
{
    Uint32_t mid = (Uint32_t)(0xFFFF + maxn) / 2;
    for (Uint32_t i = 0xFFFF; i < mid; i += 2)
        if (isPrime(i) == True)
            Prime[primeNum++] = i;
}

int main(int argc, char *argv[])
{
    maxn = (Uint32_t)atoi(argv[1]);
    Bool flag = True;
    if (maxn <= MAX_PRIME)
        checkPrime(maxn);
    else
    {
        checkPrime(MAX_PRIME);
        flag = False;
        basicNum = primeNum;
    }
    if (flag == False)
    {
        pthread_t worker_tid;
        pthread_create(&worker_tid, NULL, Slave_Thread, NULL);
        Master_Thread();
        pthread_join(worker_tid, NULL);
        for (int i = 0; i < SlaveNum; ++i)
            Prime[primeNum++] = SlavePrime[i];
    }
    for (int i = 0; i < primeNum; i++)
        printf("%d\n", Prime[i]);
    return 0;
}