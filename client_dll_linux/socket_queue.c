//
// Created by hugoc on 09/06/2021.
//



#include "socket_queue.h"


int SocketQueue_Init(pSOCKET_QUEUE pSocketQueue){
    DEBUG(pFile, "Entering\n");


    pSocketQueue->size=0;
    pSocketQueue->head=0;
    pSocketQueue->tail=-1;

    if(NULL == (pSocketQueue->buf = malloc(MAX_SOCKET_QUEUE_SIZE))){
        DEBUG(pErrorFile, "Memory allocation error : pSocketQueue->buf\n");

        return(ERROR_MEMORY);
    }
    DEBUG_VAR(pFile,  "queue buf : %p\n", pSocketQueue->buf);

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

int SocketQueue_Enqueue(pSOCKET_QUEUE pSocketQueue, unsigned char * buf, int length){
    DEBUG(pFile, "Entering\n");
    DEBUG(pSQueueFile, "Entering\n");
    if(pSocketQueue->size + length > MAX_SOCKET_QUEUE_SIZE){
        DEBUG(pErrorFile,"Queue Overflow\n");
        DEBUG(pFile,"Queue Overflow\n");
        return(ERROR_OVERFLOW);
    }

    if(length > MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1){
        memcpy(pSocketQueue->buf+pSocketQueue->tail+1, buf, MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1);
        memcpy(pSocketQueue->buf, buf+(MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1), length -(MAX_SOCKET_QUEUE_SIZE - pSocketQueue->tail - 1));
    }
    else{
        memcpy(pSocketQueue->buf+pSocketQueue->tail+1, buf, length);
    }

    pSocketQueue->tail = (pSocketQueue->tail+length) % MAX_SOCKET_QUEUE_SIZE;
    pSocketQueue->size += length;

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

int SocketQueue_Dequeue(pSOCKET_QUEUE pSocketQueue, int length){
    DEBUG(pFile, "Entering\n");
    DEBUG(pSQueueFile, "Entering\n");

    int n = MIN(pSocketQueue->size, length);

    pSocketQueue->head = (pSocketQueue->head + n) % MAX_SOCKET_QUEUE_SIZE;
    pSocketQueue->size -= n;

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

__attribute__((unused)) unsigned char * SocketQueue_GetHead(pSOCKET_QUEUE pSocketQueue){
    DEBUG(pFile, "Entering\n");
    DEBUG(pFile, "Leaving\n");
    return (pSocketQueue->buf+pSocketQueue->head);
}

int SocketQueue_GetN(pSOCKET_QUEUE pSocketQueue, int n, unsigned char ** ppOutBuf){
    if(n > MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head){
        *ppOutBuf = pSocketQueue->buf+pSocketQueue->head;
        return(MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head);
    }
    else{
        *ppOutBuf = pSocketQueue->buf+pSocketQueue->head;
        return (n);
    }
}
//int SocketQueue_Send(pSOCKET_QUEUE pSocketQueue, int length, int fd){
//    DEBUG(pFile, "Entering\n");
//    DEBUG(pSQueueFile, "Entering\n");
//
//    int n = MIN(pSocketQueue->size, length);
//    int rc;
//    int rc2;
//
//
//
//    if(n > MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head){
//        if(-1 == (rc=(int)send(fd, pSocketQueue->buf+pSocketQueue->head, MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head, 0))){
//            DEBUG(pErrorFile, "Error when trying to send a queue packet\n");
//            DEBUG(pFile, "Error when trying to send a queue packet\n");
//            return (FAILURE);
//        }
//        if( rc < MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head){
//            SocketQueue_Dequeue(pSocketQueue, rc);
//            return(rc);
//        }
//        if ((rc2=(int)send(fd, pSocketQueue->buf,  length -(MAX_SOCKET_QUEUE_SIZE - pSocketQueue->head), 0))==-1){
//            DEBUG(pFile, "Error when trying to send a queue packet\n");
//            DEBUG(pErrorFile, "Error when trying to send a queue packet\n");
//
//            return (rc);
//        };
//        SocketQueue_Dequeue(pSocketQueue, rc+rc2);
//        return (rc+rc2);
//    }
//    else {
//
//        if (-1 == (rc=(int)send(fd, pSocketQueue->buf + pSocketQueue->head, n, 0))) {
//            DEBUG(pErrorFile, "Error when trying to send a queue packet\n");
//            DEBUG(pFile, "Error when trying to send a queue packet\n");
//            return (FAILURE);
//        }
//        SocketQueue_Dequeue(pSocketQueue, rc);
//    }
//
//    DEBUG(pFile, "Leaving\n");
//    return(rc);
//}

//int SocketQueue_SendAll(pSOCKET_QUEUE pSocketQueue, int fd){
//    DEBUG(pFile, "Entering\n");
//
//
//    while(pSocketQueue->size > 0){
//        if(-1 == SocketQueue_Send(pSocketQueue, 100, fd)){
//            DEBUG(pErrorFile, "Error when trying to send a queue packet\n");
//            DEBUG(pSQueueFile, "Error when trying to send a queue packet\n");
//            return (ERROR_SEND);
//        }
//    }
//    DEBUG(pFile, "Leaving\n");
//
//    return(SUCCESS);
//}

void SocketQueue_Free(pSOCKET_QUEUE pSocketQueue){
    DEBUG(pFile, "Entering\n");
    DEBUG_VAR(pFile, "queue=%p, buf=%p\n", pSocketQueue, pSocketQueue->buf);

    if(pSocketQueue->buf){
        DEBUG(pFile, "here\n");

        free(pSocketQueue->buf);
        DEBUG(pFile, "there\n");
        pSocketQueue->buf = NULL;
    }


    DEBUG(pFile, "Leaving\n");

}

int SocketQueue_IsEmpty(pSOCKET_QUEUE pSocketQueue){
    return (pSocketQueue->size==0);
}