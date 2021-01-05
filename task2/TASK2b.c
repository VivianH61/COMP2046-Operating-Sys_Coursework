#include "coursework.h"
#include "linkedlist.h"
#include <stdlib.h>

//print the whole priority list on the screen
void print_pq(struct element ** pheads, struct element ** ptails, int * isEmpty) {
    for (int i = 0; i < MAX_PRIORITY; i ++) {
        if (isEmpty[i] == 1) {
            struct element * cur = (struct element *)pheads[i];
            printf("Priority %d:\n", i);
            while (cur != NULL) {
                struct process *tmp_p = (struct process *)(cur->pData);
                printf("\tProcess Id = %d, Priority = %d, Initial Burst Time = %d, Remaining Burst Time = %d\n", tmp_p->iProcessId, tmp_p->iPriority, tmp_p->iInitialBurstTime, tmp_p->iRemainingBurstTime);
                cur = cur->pNext;
            }
        }
    }
    printf("END\n");
}

//return 1 if the input array is all 0, retun 0 otherwise
int allEmpty(int * isEmpty, int length) {
    for (int i = 0; i < length; i ++) {
        if (isEmpty[i] == 1)
            return 0;
    }
    return 1;
}

void removeElem(struct element ** phead, struct element ** ptail, int index) {
    if (*phead == NULL)
        return;
    if (index == 0) {
        removeFirst(phead, ptail);
        return;
    }
    
    struct element * cur_prev = (*phead);
    struct element * cur = (*phead)->pNext;
    for (int i = 0; i < index - 1; i ++) {
        cur_prev = cur;
        cur = cur->pNext;
    }
    if (cur == NULL)
        return;
    if (cur -> pNext == NULL) {
        free(cur);
        cur_prev ->pNext = NULL;
        *ptail = cur_prev;
        return;
    }
    
    struct element *next = cur->pNext->pNext;
    free(cur->pNext);
    cur->pNext = next;
    if (next == NULL) {
        *ptail = cur;
    }
    return;
}

int main(int argc, char *argv[]) {
    struct element e;
    struct element * pheads[MAX_PRIORITY];
    struct element * ptails[MAX_PRIORITY];

    struct timeval oStartTime;
    struct timeval oEndTime;
    int burstT;
   

    for (int i = 0; i < MAX_PRIORITY; i ++) {
        pheads[i] = NULL;
        ptails[i] = NULL;
    }

    int isEmpty[MAX_PRIORITY] = {0};//0 for empty list and 1 for non-Empty list

    for (int i = 0; i < NUMBER_OF_PROCESSES; i ++) {
        struct process *p = generateProcess();
        
        if (isEmpty[p->iPriority] == 0) {
            pheads[p->iPriority] = NULL;
            pheads[p->iPriority] = NULL;
            isEmpty[p->iPriority] = 1;
        }
        addLast(p, &(pheads[p->iPriority]), &(ptails[p->iPriority]));
    }
    //print out the initial priority queues of the precesses
    for (int i = 0; i < MAX_PRIORITY; i ++) {
        if (isEmpty[i] == 1) {
            struct element * cur = (struct element *)pheads[i];
            printf("Priority %d:\n", i);
            while (cur != NULL) {
                struct process *tmp_p = (struct process *)(cur->pData);
                printf("\tProcess Id = %d, Priority = %d, Initial Burst Time = %d, Remaining Burst Time = %d\n", tmp_p->iProcessId, tmp_p->iPriority, tmp_p->iInitialBurstTime, tmp_p->iRemainingBurstTime);
                cur = cur->pNext;
            }
        }
    }
    printf("END\n");


    int flag;//1 for part of this process has been run; 0 for this process never been executed
    for (int i = 0; i < MAX_PRIORITY; i ++) {
        flag = 0;
     
        int index = 0;
        while (pheads[i] != NULL) {
            struct element * cur = pheads[i];
            index = 0;
            while (cur != NULL) {
                //print_pq(pheads, ptails, isEmpty);
                struct process * tmp_p = (struct process *) (cur -> pData);
                cur = cur->pNext;
                
                runPreemptiveJob(tmp_p, &oStartTime, &oEndTime);
                burstT = getDifferenceInMilliSeconds(oStartTime, oEndTime);
                printf("Process Id = %d, Priority = %d, ", tmp_p->iProcessId, i);
                printf("Previous Burst Time = %d, ", tmp_p->iPreviousBurstTime);
                printf("Remaining Burst Time = %d", tmp_p->iRemainingBurstTime);
                if (flag == 0) {
                    flag = 1;
                   
                    if (tmp_p->iRemainingBurstTime == 0) {
                        printf(", Response Time = %ld, ", getDifferenceInMilliSeconds(tmp_p->oTimeCreated, oStartTime));
                        printf("Turnaround Time = %ld\n", getDifferenceInMilliSeconds(tmp_p->oTimeCreated, oEndTime));
                        removeElem(&pheads[i], &ptails[i], index);
                        
                        
                    } else {
                        printf(", Response Time = %ld\n", getDifferenceInMilliSeconds(tmp_p->oTimeCreated, oStartTime));
                        index ++;
                    }
                    
                } else {
                   
                    if (tmp_p->iRemainingBurstTime == 0) {
                        printf(", Turnaround Time = %ld\n", getDifferenceInMilliSeconds(tmp_p->oTimeCreated, oEndTime));  
                        removeElem(&pheads[i], &ptails[i], index);
                        
                    } else {
                        printf("\n");
                        index ++;
                    }
                    
                    
                } 
                
                
            }

            
        }
    }
 
}