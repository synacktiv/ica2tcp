//
// Created by hugoc on 09/06/2021.
//

#ifndef SOCKET_QUEUE_WIN_H
#define SOCKET_QUEUE_WIN_H

#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <winsock2.h>

#include "debug_win.h"
#include "errors_win.h"

#define MAX_SOCKET_QUEUE_SIZE 100000

typedef struct S_SOCKET_QUEUE{
    int size;
    int head;
    int tail;
    unsigned char * buf;
}SOCKET_QUEUE, * pSOCKET_QUEUE;

int SocketQueue_Init(pSOCKET_QUEUE);
int SocketQueue_Enqueue(pSOCKET_QUEUE, unsigned char *, int);
int SocketQueue_Dequeue(pSOCKET_QUEUE, int);
unsigned char * SocketQueue_GetHead(pSOCKET_QUEUE);
int SocketQueue_Send(pSOCKET_QUEUE, int, int);
int SocketQueue_SendAll(pSOCKET_QUEUE, int);
void SocketQueue_Free(pSOCKET_QUEUE);
int SocketQueue_IsEmpty(pSOCKET_QUEUE);
int SocketQueue_GetN(pSOCKET_QUEUE, int, unsigned char **);

#endif //SOCKET_QUEUE_WIN_H
