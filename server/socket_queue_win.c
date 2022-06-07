//
// Created by hugoc on 09/06/2021.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "socket_queue_win.h"



int SocketQueue_Init(pSOCKET_QUEUE pSocketQueue) {
	DEBUG("Entering\n");


	pSocketQueue->size = 0;
	pSocketQueue->head = 0;
	pSocketQueue->tail = -1;

	if (NULL == (pSocketQueue->buf = malloc(MAX_SOCKET_QUEUE_SIZE))) {
		DEBUG("Memory allocation error : pSocketQueue->buf\n");
		return(ERROR_MEMORY);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int SocketQueue_Enqueue(pSOCKET_QUEUE pSocketQueue, unsigned char * buf, int length) {
	DEBUG("Entering\n");
	if (pSocketQueue->size + length > MAX_SOCKET_QUEUE_SIZE) {
		DEBUG("Queue Overflow\n");

		return(ERROR_OVERFLOW);
	}

	if (length > MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1) {
		memcpy(pSocketQueue->buf + pSocketQueue->tail + 1, buf, MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1);
		memcpy(pSocketQueue->buf, buf + (MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1), length - (MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1));
	}
	else {
		memcpy(pSocketQueue->buf + pSocketQueue->tail + 1, buf, length);
	}

	pSocketQueue->tail = (pSocketQueue->tail + length) % MAX_SOCKET_QUEUE_SIZE;
	pSocketQueue->size += length;

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int SocketQueue_Dequeue(pSOCKET_QUEUE pSocketQueue, int length) {
	DEBUG("Entering\n");
	int n = min(pSocketQueue->size, length);

	pSocketQueue->head = (pSocketQueue->head + n) % MAX_SOCKET_QUEUE_SIZE;
	pSocketQueue->size -= n;
	
	DEBUG( "Leaving\n");
	return(SUCCESS);
}

unsigned char * SocketQueue_GetHead(pSOCKET_QUEUE pSocketQueue) {
	DEBUG("Entering\n");
	DEBUG("Leaving\n");
	return (pSocketQueue->buf + pSocketQueue->head);
}

int SocketQueue_Send(pSOCKET_QUEUE pSocketQueue, int length, int fd) {
	DEBUG("Entering\n");
	int n = min(pSocketQueue->size, length);
	int rc;
	int rc2;



	if (n > MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head) {
		if (-1 == (rc = (int)send(fd, pSocketQueue->buf + pSocketQueue->head, MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head, 0))) {
			DEBUG("Error when trying to send a queue packet\n");
			return (FAILURE);
		}
		if (rc < MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head) {
			SocketQueue_Dequeue(pSocketQueue, rc);
			return(rc);
		}
		if ((rc2 = (int)send(fd, pSocketQueue->buf, length - (MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head), 0)) == -1) {
			DEBUG("Error when trying to send a queue packet\n");

			return (rc);
		};
		SocketQueue_Dequeue(pSocketQueue, rc + rc2);
		return (rc + rc2);
	}
	else {

		if (-1 == (rc = (int)send(fd, pSocketQueue->buf + pSocketQueue->head, n, 0))) {
			DEBUG("Error when trying to send a queue packet\n");
			return (FAILURE);
		}
		SocketQueue_Dequeue(pSocketQueue, rc);
	}

	DEBUG("Leaving\n");
	return(rc);
}

int SocketQueue_SendAll(pSOCKET_QUEUE pSocketQueue, int fd) {
	DEBUG("Entering\n");
	while (pSocketQueue->size > 0) {
		if (-1 == SocketQueue_Send(pSocketQueue, 100, fd)) {
			DEBUG("Error when trying to send a queue packet\n");
			return (ERROR_SEND);
		}
	}
	DEBUG("Leaving\n");

	return(SUCCESS);
}

void SocketQueue_Free(pSOCKET_QUEUE pSocketQueue) {
	DEBUG("Entering\n");
	DEBUG_VAR("queue=%p, buf=%p\n", pSocketQueue, pSocketQueue->buf);

	if (pSocketQueue->buf) {
		free(pSocketQueue->buf);
		pSocketQueue->buf = NULL;
	}
	

	DEBUG("Leaving\n");

}

int SocketQueue_IsEmpty(pSOCKET_QUEUE pSocketQueue) {
	return(pSocketQueue->size == 0);
}

int SocketQueue_GetN(pSOCKET_QUEUE pSocketQueue, int n, unsigned char ** ppOutBuf) {
	if (n > MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head) {
		*ppOutBuf = pSocketQueue->buf + pSocketQueue->head;
		return(MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head);
	}
	else {
		*ppOutBuf = pSocketQueue->buf + pSocketQueue->head;
		return(n);
	}
}