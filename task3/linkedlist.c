#include <stdlib.h>
#include "linkedlist.h"
#include "coursework.h"

void addLast(void * pData, struct element ** pHead, struct element ** pTail)
{
	struct element * pNewElement = (struct element *) malloc (sizeof(struct element));
	pNewElement->pData = pData;
	pNewElement->pNext = NULL;
	
	if((*pHead) == NULL)
	{
		pNewElement->pNext = (*pHead);
		(*pTail) = (*pHead) = pNewElement;
	}
	else 
	{
		pNewElement->pNext = NULL;
		(*pTail)->pNext = pNewElement;
		(*pTail) = pNewElement;
	}
}

void addFirst(void * pData, struct element ** pHead, struct element ** pTail)
{
	struct element * pNewElement = (struct element *) malloc (sizeof(struct element));
	pNewElement->pData = pData;
	pNewElement->pNext = NULL;
	
	if((*pHead) == NULL)
	{
		pNewElement->pNext = (*pHead);
		(*pTail) = (*pHead) = pNewElement;
	}
	else 
	{
		pNewElement->pNext = (*pHead);
		(*pHead) = pNewElement;
	}
}

void * removeFirst(struct element ** pHead, struct element ** pTail)
{
	void * pData = NULL;
	struct element * pTemp = NULL;
	if((*pHead) != NULL)
	{
		pTemp = (*pHead);
		(*pHead) = (*pHead)->pNext;
		if((*pHead) == NULL)
			(*pTail) = NULL;
		pData =  pTemp->pData;
		free(pTemp);
	}
	return pData;
}

///remember to remove!
void printlist(struct element * phead) {
	struct element * cur;
	struct process * pcur;
	while (cur -> pNext != NULL) {
		pcur = cur->pData;
		printf("Process Id = %d, InitialBT = %d, RemainBT = %d, Priority = %d\n", pcur->iProcessId, pcur->iInitialBurstTime, pcur->iRemainingBurstTime, pcur->iPriority);
		cur = cur->pNext;
	}
}
