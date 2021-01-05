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

//insert the new pproduced process into the PQ maintaining the PQ property
void pq_insert(struct process * p, struct element **phead, struct element **ptail) {
    struct element * ph = *phead;
    if (ph == NULL) {
        addLast(p, phead, ptail);
        return;
    }

    struct element * new_e = malloc(sizeof(struct element));
    new_e->pData = p;

    struct element * cur_prev = NULL;
    struct element * cur = *phead;
    struct process * pcur = (struct process *) cur->pData;


    while (cur != NULL) {
        pcur = (struct process *) cur->pData;
        if (pcur->iPriority > p->iPriority) {
            if (cur_prev == NULL) {
                new_e->pNext = cur;
                *phead = new_e;
                return;
            }
            cur_prev -> pNext = new_e;
            new_e->pNext = cur;
            return;
        }
        cur_prev = cur;
        cur = cur->pNext;
       
    }
    addLast(p, phead, ptail);
    return;
}



void * producing(void * arg) {
    int * index = (int *) arg;
    
    while (pcount < MAX_NUMBER_OF_JOBS) {    
        sem_wait(&empty);
        pthread_mutex_lock(&buffer_access);
        struct process * p = generateProcess();
        pq_insert(p, &phead, &ptail);
        printf("Producer %d, Items Produced %d, New Process Id = %d, Burst Time = %d\n", *index, ++pcount, pcount, p->iInitialBurstTime);
        pthread_mutex_unlock(&buffer_access);
        sem_post(&full);
    }
    return NULL;
    
}

void * consuming(void * arg) {
    int * index = (int *) arg;
    int flag;//1 for part of this process has been run; 0 for this process never been executed
    
    while (ccount > 0) {
        sem_wait(&full);
        pthread_mutex_lock(&buffer_access);

        //
        struct process * p = (struct process *) phead->pData;
        
        struct timeval oStartTime;
        struct timeval oEndTime;
        runPreemptiveJob(p, &oStartTime, &oEndTime);
        pthread_mutex_unlock(&buffer_access);
        

        

        //this process is done 
        if (p->iRemainingBurstTime == 0) {
            //this process has never been proceeded
            if (p->iRemainingBurstTime + TIME_SLICE >= p->iInitialBurstTime) {
                printf("Consumer = %d, Process Id = %d, Priority = %d, Previous Burst Time = %ld, New Burst Time = %ld, Respond Time = %ld, Turn Around Time = %ld\n", *index, p->iProcessId, p->iPriority, p->iPreviousBurstTime,  p->iPreviousBurstTime - getDifferenceInMilliSeconds(oStartTime, oEndTime), getDifferenceInMilliSeconds(p->oTimeCreated, oStartTime), getDifferenceInMilliSeconds(p->oTimeCreated, oEndTime));
                
               
            } else {
                printf("Consumer = %d, Process Id = %d, Priority = %d, Previous Burst Time = %ld, New Burst Time = %ld, Turn Around Time = %ld\n", *index, p->iProcessId, p->iPriority, p->iPreviousBurstTime,  p->iPreviousBurstTime - getDifferenceInMilliSeconds(oStartTime, oEndTime), getDifferenceInMilliSeconds(p->oTimeCreated, oEndTime));
               
            }
            pthread_mutex_lock(&buffer_access);
            removeFirst(&phead, &ptail);
            ccount --;
            pthread_mutex_unlock(&buffer_access);
            sem_post(&empty);
        } else {
            if (p->iRemainingBurstTime + TIME_SLICE >= p->iInitialBurstTime) {
                printf("Consumer = %d, Process Id = %d, Priority = %d, Previous Burst Time = %ld, New Burst Time = %ld", *index, p->iProcessId, p->iPriority, p->iPreviousBurstTime,  p->iPreviousBurstTime - getDifferenceInMilliSeconds(oStartTime, oEndTime));
                printf(", Respond Time = %ld\n", getDifferenceInMilliSeconds(p->oTimeCreated, oStartTime));
            } else {
                printf("Consumer = %d, Process Id = %d, Priority = %d, Previous Burst Time = %ld, New Burst Time = %ld\n", *index, p->iProcessId, p->iPriority, p->iPreviousBurstTime,  p->iPreviousBurstTime - getDifferenceInMilliSeconds(oStartTime, oEndTime));
            }
            sem_post(&full);
        }
        
    }

    return NULL;
}

void main(int argc, char* argv[]) {
    for (int i = 0; i < NUMBER_OF_PRODUCERS; i ++) {
        pindex[i] = i;
    }
        
    for (int i = 0; i < NUMBER_OF_CONSUMERS; i ++) {
        cindex[i] = i;
    }
        

    sem_init(&full, 0, 0);
    sem_init(&empty, 0, MAX_BUFFER_SIZE);
    
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

}
