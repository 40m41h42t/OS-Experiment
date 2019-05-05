#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

#define CAPACITY 4
#define ITEM_COUNT (CAPACITY * 2)
#define buffer1_empty (buffer1_in == buffer1_out)
#define buffer1_full ((buffer1_in + 1) % CAPACITY == buffer1_out)
#define buffer2_empty (buffer2_in == buffer2_out)
#define buffer2_full ((buffer2_in + 1) % CAPACITY == buffer2_out)

int buffer1[CAPACITY];
int buffer2[CAPACITY];
int buffer1_in, buffer1_out;
int buffer2_in, buffer2_out;

pthread_mutex_t buffer1_mutex;
pthread_cond_t wait_buffer1_empty;
pthread_cond_t wait_buffer1_full;
pthread_mutex_t buffer2_mutex;
pthread_cond_t wait_buffer2_empty;
pthread_cond_t wait_buffer2_full;

void init();
void *producer(void *arg);
void *calculator(void *arg);
void *consumer(void *arg);
void put_item2buffer1(char item);
char get_item4buffer1();
void to_upper_buffer2(char item);
char get_item4buffer2();
int main()
{
    init();
    pthread_t consumer_tid, calculator_tid;
    pthread_create(&consumer_tid, NULL, consumer, NULL);
    pthread_create(&calculator_tid, NULL, calculator, NULL);
    producer(NULL);
    pthread_join(consumer_tid, NULL);
    return 0;
}

void init()
{
    buffer1_in = buffer1_out = 0;
    buffer2_in = buffer2_out = 0;
    pthread_mutex_init(&buffer1_mutex, NULL);
    pthread_cond_init(&wait_buffer1_empty, NULL);
    pthread_cond_init(&wait_buffer1_full, NULL);
    pthread_mutex_init(&buffer2_mutex, NULL);
    pthread_cond_init(&wait_buffer2_empty, NULL);
    pthread_cond_init(&wait_buffer2_full, NULL);
}
void put_item2buffer1(char item)
{
    buffer1[buffer1_in] = item;
    buffer1_in = (buffer1_in + 1) % CAPACITY;
}
char get_item4buffer1()
{
    char item;
    item = buffer1[buffer1_out];
    buffer1_out = (buffer1_out + 1) % CAPACITY;
    return item;
}
void to_upper_buffer2(char item)
{
    buffer2[buffer2_in] = toupper(item);
    printf("  calculate item: toupper %c\n", buffer2[buffer2_in]);
    buffer2_in = (buffer2_in + 1) % CAPACITY;
}
char get_item4buffer2()
{
    char item;
    item = buffer2[buffer2_out];
    buffer2_out = (buffer2_out + 1) % CAPACITY;
    return item;
}
// 生产者
void *producer(void *arg)
{
    int i;
    char item;
    for (i = 0; i < ITEM_COUNT; ++i)
    {
        pthread_mutex_lock(&buffer1_mutex);
        while (buffer1_full)
            pthread_cond_wait(&wait_buffer1_empty, &buffer1_mutex);
        item = 'a' + i;
        put_item2buffer1(item);
        printf("produce item: %c\n", item);
        pthread_cond_signal(&wait_buffer1_full);
        pthread_mutex_unlock(&buffer1_mutex);
    }
    return NULL;
}

void *calculator(void *arg)
{
    int i;
    char item;
    for (i = 0; i < ITEM_COUNT; ++i)
    {
        pthread_mutex_lock(&buffer1_mutex);
        while (buffer1_empty)
            pthread_cond_wait(&wait_buffer1_full, &buffer1_mutex);
        item = get_item4buffer1();
        printf("  calculate item: bring %c\n", item);
        pthread_cond_signal(&wait_buffer1_empty);
        pthread_mutex_unlock(&buffer1_mutex);
        pthread_mutex_lock(&buffer2_mutex);
        while (buffer2_full)
            pthread_cond_wait(&wait_buffer2_empty, &buffer2_mutex);
        to_upper_buffer2(item);
        pthread_cond_signal(&wait_buffer2_full);
        pthread_mutex_unlock(&buffer2_mutex);
    }
    return NULL;
}
void *consumer(void *arg)
{
    int i;
    char item;
    for (i = 0; i < ITEM_COUNT; ++i)
    {
        pthread_mutex_lock(&buffer2_mutex);
        while (buffer2_empty)
            pthread_cond_wait(&wait_buffer2_full, &buffer2_mutex);
        item = get_item4buffer2();
        printf("    consume item: %c\n", item);
        pthread_cond_signal(&wait_buffer2_empty);
        pthread_mutex_unlock(&buffer2_mutex);
    }
    return NULL;
}