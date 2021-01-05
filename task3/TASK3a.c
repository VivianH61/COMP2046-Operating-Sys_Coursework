#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
#include <semaphore.h>
#include "coursework.h"
#include "linkedlist.h"

pthread_t producers_id[NUMBER_OF_PRODUCERS] = {0};

pthread_t consumers_id[NUMBER_OF_CONSUMERS] = {0};

//the pointer pointing to the head of the linked list for the bounded buffer
struct element * phead = NULL;
//the pointer pointing to the last element of the linked list representing the bounder buffer
struct element * ptail = NULL;
//counting the number of processes that is produced by the producers
int pcount = 0;
//counting the remaining number of process to process by the consumers
int ccount = MAX_NUMBER_OF_JOBS;

int pindex[NUMBER_OF_PRODUCERS] = {0};
int cindex[NUMBER_OF_CONSUMERS] = {0};

int respondTSum;
int turnaroundTSum;

//block other threads when one thread is accessing the buffer
pthread_mutex_t buffer_access;
//sem_t buffer_access;
//counting semaphore keeping track of the number of full buffers, initialised to 0
sem_t full;
//counting semaphore keeping track of the number of empty buffers, initialised to MAX_BUFFER_SIZE
sem_t empty;

//return the current size of the bounded buffer
int bufferSize(struct element * phead);

void * producing(void * arg) {
    int * index = (int *) arg;
    
    while (pcount < MAX_NUMBER_OF_JOBS) {
        
        sem_wait(&empty);
        
        pthread_mutex_lock(&buffer_access);
        struct process * p = generateProcess();
        addLast(p, &phead, &ptail);
        printf("Producer %d, Items Produced %d, New Process Id = %d, Burst Time = %d\n", *index, ++pcount, pcount, p->iInitialBurstTime);
        pthread_mutex_unlock(&buffer_access);
        
        sem_post(&full);
    }
    return NULL;
    
}

void * consuming(void * arg) {
    int * index =  (int *) arg;

    while (ccount > 0) {
        
        sem_wait(&full);
        pthread_mutex_lock(&buffer_access);
 
        struct process * p = removeFirst(&phead, &ptail);
        ccount --;

        pthread_mutex_unlock(&buffer_access);
        struct timeval start;
        struct timeval end;
        runNonPreemptiveJob(p, &start, &end);
        //
        sem_post(&empty);
        //
        printf("Consumer = %d, ", *index);
        printf("Process Id = %d, ", p->iProcessId);
        printf("Previous Burst Time = %ld, ", getDifferenceInMilliSeconds(start, end));
        printf("New Burst Time = %ld, ", p->iPreviousBurstTime - getDifferenceInMilliSeconds(start, end));
        respondTSum += getDifferenceInMilliSeconds(p->oTimeCreated, start);
        printf("Respond Time = %ld, ", getDifferenceInMilliSeconds(p->oTimeCreated, start));
        turnaroundTSum += getDifferenceInMilliSeconds(p->oTimeCreated, end);
        printf("Turn Around Time = %ld\n", getDifferenceInMilliSeconds(p->oTimeCreated, end));
        
    }
    
    return NULL;
}

int main(int argc, char * argv[]) {
    for (int i = 0; i < NUMBER_OF_PRODUCERS; i ++) {
        pindex[i] = i;
    }
        
    for (int i = 0; i < NUMBER_OF_CONSUMERS; i ++) {
        cindex[i] = i;
    }
        

    sem_init(&full, 0, 0);
    sem_init(&empty, 0, MAX_BUFFER_SIZE);
    //sem_init(&buffer_access, 0, 1);
    pthread_mutex_init(&buffer_access, NULL);

    respondTSum = 0;
    turnaroundTSum = 0;

    for (int i = 0; i < NUMBER_OF_PRODUCERS; i ++) {
        pthread_create(&producers_id[i], NULL, producing, &pindex[i]);
    }
 

    for (int i = 0; i < NUMBER_OF_CONSUMERS; i ++) {
        pthread_create(&consumers_id[i], NULL, consuming, &cindex[i]);
    }

    for (int i = NUMBER_OF_PRODUCERS - 1; i >= 0; i --) {
        pthread_join(producers_id[i], NULL);
    }

    for (int i = NUMBER_OF_CONSUMERS - 1; i >= 0; i --) {
        pthread_join(consumers_id[i], NULL);
    }

    sem_destroy(&full);
    
    sem_destroy(&empty);
    
    //sem_destroy(&buffer_access);
    pthread_mutex_destroy(&buffer_access);

    printf("Average response time = %f\n", (double)respondTSum / MAX_NUMBER_OF_JOBS);
    printf("Average turn around time = %f\n", (double)turnaroundTSum / MAX_NUMBER_OF_JOBS);

    return 0;
}






