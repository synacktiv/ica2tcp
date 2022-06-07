

#ifndef VDSOCKS_V2_ICA_QUEUE_H
#define VDSOCKS_V2_ICA_QUEUE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef SDK
#include <citrix/wdapi.h> // for MEMORY_SECTION
#else
#include <wdapi.h>
#endif

#include "debug.h"
#include "debug_extern.h"
#include "errors.h"

#define MAX_ICA_PACKET_DATA_SIZE 2048
#define MAX_QUEUE_SIZE 5000

typedef struct S_ICA_PACKET_QUEUE{
    int size;
    int head;
    int tail;
    MEMORY_SECTION * sections ;
    unsigned char * data_storage;
}ICA_PACKET_QUEUE, * pICA_PACKET_QUEUE;


#endif //VDSOCKS_V2_ICA_QUEUE_H
