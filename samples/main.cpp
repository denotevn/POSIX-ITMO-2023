#include<string>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<time.h>
#include <iostream>
#include <cstdlib>

#define THREAD_NUM 8

// Semaphore là một cấu trúc dữ liệu, được dùng để đồng bộ tài nguyên và đồng bộ hoạt động.
sem_t semEmpty;
sem_t semFull;


int buffer[10];
int count = 0;

pthread_mutex_t mutexBuffer;

void * producer(void *args)
{
    while (1)
    {
        int x = rand() % 100;
        sleep(1);

        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        buffer[count] = x;
        count++;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }
    
}

void* consumer(void* args)
{
    while(1)
    {
        int y;
        // remove from the buffer
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        y = buffer[count-1];
        count --;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);

        // Comsume
        printf("Got %d\n", y);
        // sleep(1);
    }
    
    
}


int main(int argc, char* argv[])
{
    srand(time(NULL));
    pthread_t th[THREAD_NUM];

    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);
    int i;
    for(i = 0; i < THREAD_NUM; i++)
    {
        if(i % 2 == 0)
        {
            if(pthread_create(&th[i], NULL, &producer, NULL) != 0)
            {
                perror("failed to create thread!");
            }
        }else{
            if(pthread_create(&th[i], NULL, &consumer, NULL) != 0)
            {
                perror("Failed to create thread! ");
            }
        }
    }

    for (i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join thread!");
        }
    }

    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);

    return 0;
}

