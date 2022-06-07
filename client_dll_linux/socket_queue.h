//
// Created by hugoc on 09/06/2021.
//

#ifndef VDSOCKS_V2_SOCKET_QUEUE_H
#define VDSOCKS_V2_SOCKET_QUEUE_H

#define NULL ((void *) 0)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/param.h>

#include "errors.h"
#include "debug.h"
#include "debug_extern.h"

#define MAX_SOCKET_QUEUE_SIZE 100000




typedef struct S_SOCKET_QUEUE{
    int size;
    int head;
    int tail;
    unsigned char * buf;
}SOCKET_QUEUE, * pSOCKET_QUEUE;

int SocketQueue_Init(pSOCKET_QUEUE pSocketQueue);
int SocketQueue_Enqueue(pSOCKET_QUEUE pSocketQueue, unsigned char * buf, int length);
int SocketQueue_Dequeue(pSOCKET_QUEUE pSocketQueue, int length);
int SocketQueue_Send(pSOCKET_QUEUE pSocketQueue, int length, int fd);
int SocketQueue_SendAll(pSOCKET_QUEUE pSocketQueue, int fd);
void SocketQueue_Free(pSOCKET_QUEUE pSocketQueue);
int SocketQueue_IsEmpty(pSOCKET_QUEUE pSocketQueue);
int SocketQueue_GetN(pSOCKET_QUEUE pSocketQueue, int n, unsigned char ** ppOutBuf);

#endif //VDSOCKS_V2_SOCKET_QUEUE_H
