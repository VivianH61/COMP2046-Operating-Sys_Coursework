#include "coursework.h"
#include "linkedlist.h"

int main(int argc, char *argv[]) {
    //initialize a ready queue
    struct element *phead = NULL; 
    struct element *ptail = NULL;

    struct process *p;

    struct timeval oStartTime;
    struct timeval oEndTime;
   
    //turn around time = respond time + burst time
    int preBurst, newBurst;
    int respondTSum = 0;
    int turnaroundTSum = 0;
   
    for (int i = 0; i < NUMBER_OF_PROCESSES; i ++) {
        struct process *p = generateProcess();
        addLast(p, &phead, &ptail);     
    }
 
    for (int j = 0; j < NUMBER_OF_PROCESSES; j ++) {
        p = phead->pData;
        runNonPreemptiveJob(p, &oStartTime, &oEndTime); 
        
        preBurst = getDifferenceInMilliSeconds(oStartTime, oEndTime);
        newBurst =  p->iInitialBurstTime - preBurst;

        respondTSum += getDifferenceInMilliSeconds(p->oTimeCreated, oStartTime);
        turnaroundTSum += getDifferenceInMilliSeconds(p->oTimeCreated, oEndTime);
        printf("Process Id = %d,  ", p->iProcessId);  
        printf("Previous Burst Time: %d, ", preBurst);
        printf("New Burst Time: %d, ", newBurst);
        printf("Response Time: %ld, ", getDifferenceInMilliSeconds(p->oTimeCreated, oStartTime));
        printf("Turn Around Time: %ld\n", getDifferenceInMilliSeconds(p->oTimeCreated, oEndTime));
        removeFirst(&phead, &ptail);
        
    }

    printf("Average response time = %f\n", (double)respondTSum / NUMBER_OF_PROCESSES);
    printf("Average turn around time = %f\n", (double)turnaroundTSum / NUMBER_OF_PROCESSES);



    
}