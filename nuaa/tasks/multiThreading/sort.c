#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int array[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
void Select_Sort(int *array, int begin, int end)
{
    int i, j, min, tmp;
    for (i = begin; i < end - 1; ++i)
    {
        min = i;
        for (j = i + 1; j < end; ++j)
            if (array[min] > array[j])
                min = j;
        if (min != j)
        {
            tmp = array[min];
            array[min] = array[i];
            array[i] = tmp;
        }
    }
}
void *Slave_Thread(void *args)
{
    Select_Sort(array, sizeof(array) / sizeof(int) / 2, sizeof(array) / sizeof(int));
}
void Master_Thread()
{
    Select_Sort(array, 0, sizeof(array) / sizeof(int) / 2);
}
void Merge_sort(int *arr)
{
    int i = 0, j = sizeof(array) / sizeof(int) / 2;
    int count = 0;
    while (i < sizeof(array) / sizeof(int) / 2 && j < sizeof(array) / sizeof(int))
    {
        arr[count++] = array[i] < array[j] ? array[i++] : array[j++];
    }
    while (i < sizeof(array) / sizeof(int) / 2)
    {
        arr[count++] = array[i++];
    }
    while (j < sizeof(array) / sizeof(int))
    {
        arr[count++] = array[j++];
    }
}
int main()
{
    int *sortedArray = (int *)malloc(sizeof(array));
    pthread_t slaveTid;
    pthread_create(&slaveTid, NULL, Slave_Thread, NULL);
    Master_Thread();
    pthread_join(slaveTid, NULL);
    Merge_sort(sortedArray);
    for(int i=0;i<sizeof(array)/sizeof(int);++i){
        printf("%d ", sortedArray[i]);
    }
    putchar('\n');
}