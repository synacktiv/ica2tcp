//
// Created by hugoc on 09/06/2021.
//

#ifndef QUEUE_WIN_H
#define QUEUE_WIN_H

#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <winsock2.h>
#include <stdlib.h>

#include "debug_win.h"
#include "errors_win.h"

#define MAX_SOCKET_QUEUE_SIZE 100000

typedef struct S_QUEUE{
    int size;
    int head;
    int tail;
    char * buf;
}QUEUE, * pQUEUE;

int Queue_Init(pQUEUE);
int Queue_Enqueue(pQUEUE, char *, int);
int Queue_Dequeue(pQUEUE, int);
char * Queue_GetHead(pQUEUE);
int Queue_Send(pQUEUE, int, int);
int Queue_SendAll(pQUEUE, int);
void Queue_Free(pQUEUE);
int Queue_IsEmpty(pQUEUE);
int Queue_GetN(pQUEUE, int, char **);
int Queue_GetSize(pQUEUE pQueue) ;
int Queue_GetSizeCeiled(pQUEUE pQueue);

#endif //QUEUE_WIN_H
