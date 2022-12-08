//
// Created by hugoc on 09/06/2021.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "queue_win.h"



int Queue_Init(pQUEUE pQueue) {
	DEBUG("Entering\n");


	pQueue->size = 0;
	pQueue->head = 0;
	pQueue->tail = -1;

	if (NULL == (pQueue->buf = (char *)malloc(MAX_SOCKET_QUEUE_SIZE))) {
		DEBUG("Memory allocation error : pQueue->buf\n");
		return(ERROR_MEMORY);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Queue_Enqueue(pQUEUE pQueue, char * buf, int length) {
	DEBUG("Entering\n");
	if (pQueue->size + length > MAX_SOCKET_QUEUE_SIZE) {
		DEBUG("Queue Overflow\n");

		return(ERROR_OVERFLOW);
	}

	if (length > MAX_SOCKET_QUEUE_SIZE - pQueue->tail - 1) {
		memcpy(pQueue->buf + pQueue->tail + 1, buf, MAX_SOCKET_QUEUE_SIZE - pQueue->tail - 1);
		memcpy(pQueue->buf, buf + (MAX_SOCKET_QUEUE_SIZE - pQueue->tail - 1), length - (MAX_SOCKET_QUEUE_SIZE - pQueue->tail - 1));
	}
	else {
		memcpy(pQueue->buf + pQueue->tail + 1, buf, length);
	}

	pQueue->tail = (pQueue->tail + length) % MAX_SOCKET_QUEUE_SIZE;
	pQueue->size += length;

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Queue_Dequeue(pQUEUE pQueue, int length) {
	DEBUG("Entering\n");
	int n = min(pQueue->size, length);

	pQueue->head = (pQueue->head + n) % MAX_SOCKET_QUEUE_SIZE;
	pQueue->size -= n;
	
	DEBUG( "Leaving\n");
	return(SUCCESS);
}

char * Queue_GetHead(pQUEUE pQueue) {
	DEBUG("Entering\n");
	DEBUG("Leaving\n");
	return (pQueue->buf + pQueue->head);
}

int Queue_GetSizeCeiled(pQUEUE pQueue){
	if(pQueue->size > MAX_SOCKET_QUEUE_SIZE - pQueue->head){
		return(MAX_SOCKET_QUEUE_SIZE - pQueue->head);
	}
	else{
		return(pQueue->size);
	}
}

int Queue_GetSize(pQUEUE pQueue) {
	DEBUG("Entering\n");
	DEBUG("Leaving\n");
	return (pQueue->size);
}

int Queue_Send(pQUEUE pQueue, int length, int fd) {
	DEBUG("Entering\n");
	int n = min(pQueue->size, length);
	int rc;
	int rc2;



	if (n > MAX_SOCKET_QUEUE_SIZE - pQueue->head) {
		if (-1 == (rc = (int)send(fd, (const char *)(pQueue->buf + pQueue->head), MAX_SOCKET_QUEUE_SIZE - pQueue->head, 0))) {
			DEBUG("Error when trying to send a queue packet\n");
			return (FAILURE);
		}
		if (rc < MAX_SOCKET_QUEUE_SIZE - pQueue->head) {
			Queue_Dequeue(pQueue, rc);
			return(rc);
		}
		if ((rc2 = (int)send(fd, (const char *)(pQueue->buf), length - (MAX_SOCKET_QUEUE_SIZE - pQueue->head), 0)) == -1) {
			DEBUG("Error when trying to send a queue packet\n");

			return (rc);
		};
		Queue_Dequeue(pQueue, rc + rc2);
		return (rc + rc2);
	}
	else {

		if (-1 == (rc = (int)send(fd, (const char *)(pQueue->buf + pQueue->head), n, 0))) {
			DEBUG("Error when trying to send a queue packet\n");
			return (FAILURE);
		}
		Queue_Dequeue(pQueue, rc);
	}

	DEBUG("Leaving\n");
	return(rc);
}

int Queue_SendAll(pQUEUE pQueue, int fd) {
	DEBUG("Entering\n");
	while (pQueue->size > 0) {
		if (-1 == Queue_Send(pQueue, 100, fd)) {
			DEBUG("Error when trying to send a queue packet\n");
			return (ERROR_SEND);
		}
	}
	DEBUG("Leaving\n");

	return(SUCCESS);
}

void Queue_Free(pQUEUE pQueue) {
	DEBUG("Entering\n");
	DEBUG_VAR("queue=%p, buf=%p\n", pQueue, pQueue->buf);

	if (pQueue->buf) {
		free(pQueue->buf);
		pQueue->buf = NULL;
	}
	

	DEBUG("Leaving\n");

}

int Queue_IsEmpty(pQUEUE pQueue) {
	return(pQueue->size == 0);
}

int Queue_GetN(pQUEUE pQueue, int n, char ** ppOutBuf) {
	if (n > MAX_SOCKET_QUEUE_SIZE - pQueue->head) {
		*ppOutBuf = pQueue->buf + pQueue->head;
		return(MAX_SOCKET_QUEUE_SIZE - pQueue->head);
	}
	else {
		*ppOutBuf = pQueue->buf + pQueue->head;
		return(n);
	}
}

