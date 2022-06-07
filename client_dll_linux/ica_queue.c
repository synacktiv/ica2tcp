//
// Created by hugoc on 29/07/2021.
//



#include "ica_queue.h"


int IcaPacketQueue_Init(pICA_PACKET_QUEUE * ppQueue) {
    DEBUG(pFile, "Entering\n");
    if( NULL == (*ppQueue = malloc(sizeof(ICA_PACKET_QUEUE)))){
        DEBUG(pErrorFile, "Memory allocation error : pQueue\n");
        return(ERROR_MEMORY);
    }

    (*ppQueue)->size = 0;
    (*ppQueue)->head = 0;
    (*ppQueue)->tail = -1;

    if( NULL == ((*ppQueue)->sections = calloc(MAX_QUEUE_SIZE, sizeof(MEMORY_SECTION)))){
        DEBUG(pErrorFile, "Memory allocation error : pQueue->sections\n");
        return(ERROR_MEMORY);
    }

    if(NULL == ((*ppQueue)->data_storage = calloc(MAX_QUEUE_SIZE, MAX_ICA_PACKET_DATA_SIZE))){
        DEBUG(pErrorFile, "Memory allocation error : pQueue->sections\n");
        return(ERROR_MEMORY);
    }

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

int IcaPacketQueue_Enqueue(pICA_PACKET_QUEUE pQueue, MEMORY_SECTION * pMS, int nMS){
    DEBUG(pFile, "Entering\n");
    unsigned int length = 0;

    if(pQueue->size == MAX_QUEUE_SIZE){
        DEBUG(pErrorFile,"Queue Overflow\n");
        return(ERROR_OVERFLOW);
    }

    for(int i=0; i<nMS; i++){
        length+= pMS[i].length;
    }
    if(length > MAX_ICA_PACKET_DATA_SIZE){
        DEBUG(pErrorFile, "Packet overflow\n");
        return(ERROR_OVERFLOW);
    }


    pQueue->tail = (pQueue->tail + 1) % MAX_QUEUE_SIZE;

    unsigned char * p = pQueue->data_storage + (pQueue->tail*MAX_ICA_PACKET_DATA_SIZE);
    for(int i=0; i<nMS; i++){
        memcpy(p , pMS[i].pSection, pMS[i].length);
        p += pMS[i].length;
    }

    pQueue->sections[pQueue->tail].length = length;
    pQueue->sections[pQueue->tail].pSection = pQueue->data_storage + (pQueue->tail*MAX_ICA_PACKET_DATA_SIZE);
    pQueue->size++;



    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

int IcaPacketQueue_EnqueueHead(pICA_PACKET_QUEUE pQueue, MEMORY_SECTION * pMS, int nMS){
    DEBUG(pFile, "Entering\n");
    unsigned int length = 0;

    if(pQueue->size == MAX_QUEUE_SIZE){
        DEBUG(pErrorFile,"Queue Overflow\n");
        return(ERROR_OVERFLOW);
    }

    for(int i=0; i<nMS; i++){
        length+= pMS[i].length;
    }
    if(length > MAX_ICA_PACKET_DATA_SIZE){
        DEBUG(pErrorFile, "Packet overflow\n");
        return(ERROR_OVERFLOW);
    }


    pQueue->head = (pQueue->head -1 );
    if(pQueue->head == -1){
        pQueue->head == MAX_QUEUE_SIZE -1;
    }

    unsigned char * p = pQueue->data_storage + (pQueue->head*MAX_ICA_PACKET_DATA_SIZE);
    for(int i=0; i<nMS; i++){
        memcpy(p , pMS[i].pSection, pMS[i].length);
        p += pMS[i].length;
    }

    pQueue->sections[pQueue->head].length = length;
    pQueue->sections[pQueue->head].pSection = pQueue->data_storage + (pQueue->head*MAX_ICA_PACKET_DATA_SIZE);
    pQueue->size++;



    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

int IcaPacketQueue_Dequeue(pICA_PACKET_QUEUE pQueue){
    DEBUG(pFile, "Entering\n");
    if(pQueue->size==0){
        DEBUG(pErrorFile, "Queue underflow\n");
        return(ERROR_UNDERFLOW);
    }

    pQueue->head = (pQueue->head + 1) % MAX_QUEUE_SIZE;
    pQueue->size--;


    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

MEMORY_SECTION * IcaPacketQueue_GetHead(pICA_PACKET_QUEUE pQueue){
    DEBUG(pFile, "Entering\n");
    if(pQueue->size==0){
        DEBUG(pFile, "Queue is empty\n");
        return NULL;
    }

    DEBUG(pFile, "Leaving\n");
    return (pQueue->sections + (pQueue->head));
}

__attribute__((unused)) int IcaPacketQueue_IsFull(pICA_PACKET_QUEUE pQueue){
    DEBUG(pFile, "Entering\n");
    DEBUG(pFile, "Leaving\n");
    return (pQueue->size==MAX_QUEUE_SIZE);
}

int IcaPacketQueue_IsEmpty(pICA_PACKET_QUEUE pQueue){
    DEBUG(pFile, "Entering\n");
    DEBUG(pFile, "Leaving\n");
    return (pQueue->size==0);
}

int IcaPacketQueue_GetFreeSpace(pICA_PACKET_QUEUE pQueue){
    DEBUG(pFile, "Entering\n");
    DEBUG(pFile, "Leaving\n");
    return(MAX_QUEUE_SIZE - pQueue->size);
}

void IcaPacketQueue_Close(pICA_PACKET_QUEUE pQueue){
    if(pQueue->data_storage){
        free(pQueue->data_storage);
        pQueue->data_storage = NULL;
    }

    if(pQueue->sections){
        free(pQueue->sections);
        pQueue->sections = NULL;
    }

    free(pQueue);
}