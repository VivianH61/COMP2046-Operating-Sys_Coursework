#include "coursework.h"
#include "linkedlist.h"
#include <stdlib.h>


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

int main(){
    struct element e;
    struct element * phead = NULL;
    struct element * ptail = NULL;

    struct timeval oStartTime;
    struct timeval oEndTime;
    int burstT;
    int isEmpty[MAX_PRIORITY] = {0};//0 for empty list and 1 for non-Empty list
   




    for (int i = 0; i < NUMBER_OF_PROCESSES; i ++) {
        struct process *p = generateProcess();
 
        addLast(p, &phead, &ptail);
    }

    removeElem(&phead, &ptail, 9);
    removeElem(&phead, &ptail, 8);
    removeElem(&phead, &ptail, 0);
    removeElem(&phead, &ptail, 10);
}
