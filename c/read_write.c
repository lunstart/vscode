#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h> // 包含 intptr_t 类型的头文件

#define NUM_READERS 3
#define NUM_WRITERS 2

sem_t rCountMutex; // 控制对 rCount 的互斥修改，初始值为 1
sem_t wDataMutex;  // 控制写者写操作的互斥信号量，初始值为 1
sem_t flag;        // 用于实现公平竞争，初始值为 1
int rCount = 0;    // 正在进行读操作的读者个数，初始化为 0
int shared_data = 0;

void* writer(void* arg) {
    intptr_t id = (intptr_t)arg; // 使用 intptr_t 类型进行转换
    for (int i = 0; i < 2; ++i) { // 限制写操作次数
        sem_wait(&flag);
        sem_wait(&wDataMutex); // 写者写操作之间互斥，进入临界区
        printf("写者 %ld 进入临界区\n", id);

        // 模拟写入数据
        shared_data++;
        printf("写者 %ld 写入 shared_data: %d\n", id, shared_data);

        sem_post(&wDataMutex); // 离开临界区
        sem_post(&flag);
        printf("写者 %ld 离开临界区\n", id);
    }
    return NULL;
}

void* reader(void* arg) {
    intptr_t id = (intptr_t)arg; // 使用 intptr_t 类型进行转换
    for (int i = 0; i < 2; ++i) { // 限制读操作次数
        sem_wait(&flag);
        sem_wait(&rCountMutex); // 进入临界区
        if (rCount == 0) {
            sem_wait(&wDataMutex); // 当第一个读者进入，如果有写者则阻塞写者写操作
        }
        rCount++;
        sem_post(&rCountMutex); // 离开临界区
        sem_post(&flag);

        printf("读者 %ld 进入临界区\n", id);
        printf("读者 %ld 读取 shared_data: %d\n", id, shared_data);

        sem_wait(&rCountMutex); // 进入临界区
        rCount--;
        if (rCount == 0) {
            sem_post(&wDataMutex); // 当没有读者了，则唤醒阻塞中写者的写操作
        }
        sem_post(&rCountMutex); // 离开临界区

        printf("读者 %ld 离开临界区\n", id);
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    sem_init(&rCountMutex, 0, 1); // 初始化 rCountMutex 信号量
    sem_init(&wDataMutex, 0, 1);   // 初始化 wDataMutex 信号量
    sem_init(&flag, 0, 1);         // 初始化 flag 信号量

    // 创建读者线程
    for (intptr_t i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, (void*)i);
    }

    // 创建写者线程
    for (intptr_t i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer, (void*)i);
    }

    // 等待读者线程结束
    for (intptr_t i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    // 等待写者线程结束
    for (intptr_t i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    sem_destroy(&rCountMutex); // 释放 rCountMutex 信号量
    sem_destroy(&wDataMutex);  // 释放 wDataMutex 信号量
    sem_destroy(&flag);        // 释放 flag 信号量

    return 0;
}
