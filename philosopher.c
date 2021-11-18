#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int tid;
    int delay;
    int last;
} Thread;

int totalNum = 0;
// 记录程序开始运行时间
time_t startTime;
sem_t ready; // 同时能抢夺资源的剩余名额
// sem_t *hungry; // 同步哲学家开始争夺资源的信号量 
sem_t *chopsticks; // 筷子，即资源

void* philosopher(void* argPtr) {
    Thread arg = *(Thread*)argPtr;
    sleep(arg.delay);
    // printf("[%02.0lf秒]哲学家%d等待就餐\n",
    //     difftime(time(NULL), startTime), arg.tid);
    // 等待剩余名额
    sem_wait(&ready);
    printf("[%02.0lf秒]哲学家%d\x1b[33m等待就餐\x1b[0m\n",
        difftime(time(NULL), startTime), arg.tid);
    sem_wait(chopsticks + arg.tid);
    sem_wait(chopsticks + (arg.tid + 1) % totalNum);
    
    printf("[%02.0lf秒]哲学家%d\x1b[32m开始就餐\x1b[0m, 筷子%d和%d被占用\n",
        difftime(time(NULL), startTime), arg.tid, arg.tid, arg.tid + 1);
    sleep(arg.last);
    printf("[%02.0lf秒]哲学家%d\x1b[35m就餐完成\x1b[0m\n",
        difftime(time(NULL), startTime), arg.tid);

    sem_post(chopsticks + arg.tid);
    sem_post(chopsticks + (arg.tid + 1) % totalNum);
    sem_post(&ready);
    
    return 0;
}

int main()
{
    int readyNum;
    assert(scanf("%d %d", &totalNum, &readyNum) == 2);
    chopsticks = (sem_t*)malloc(sizeof(sem_t) * totalNum);
    int tidEnd = 0;

    // 哲学家进程和相关信息
    pthread_t tid[totalNum];
    Thread thread[totalNum];

    // 初始化剩余名额为设定数目
    sem_init(&ready, 0, readyNum);
    for (int i = 0; i < totalNum; i++) {
        // 初始化筷子为可以使用
        sem_init(chopsticks + i, 0, 1);
    }

    startTime = time(NULL);

    int argTid;
    int argDelay;
    int argLast;

    while(scanf("%d %d %d", &argTid, &argDelay, &argLast) == 3) {
        assert(tidEnd < totalNum);
        
        thread[tidEnd].tid = argTid;
        thread[tidEnd].delay = argDelay;
        thread[tidEnd].last = argLast;

        pthread_create(tid + tidEnd, NULL, philosopher, thread + tidEnd);
        printf("[%02.0lf秒]创建进程%d\n", difftime(time(NULL), startTime), argTid);
        ++tidEnd;
    }

    for(int i = 0; i < tidEnd; i++) {
        pthread_join(tid[i], NULL);
    }

    sem_destroy(&ready);
    for (int i = 0; i < totalNum; i++) {
        sem_destroy(chopsticks + i);
    }
    free(chopsticks);
    return 0;
}