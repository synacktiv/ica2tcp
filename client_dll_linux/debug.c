


//
// Created by hugoc on 10/06/2021.
//

#include "debug.h"

//Files for debugging and logging >> opened in DriverOpen()
FILE* pFile;
FILE* pPollFile;
FILE* pUnsentFile;
FILE* pSQueueFile;
FILE* pErrorFile;


void DebugInit(char basePath[180]){ //FIXME : check buffer overflow
    char tmp[200];
    strncpy(tmp, basePath, 180);
    pPollFile = fopen(strncat(tmp, POLL_LOG_PATH, 20), "w");
    strncpy(tmp, basePath, 180);
    pFile = fopen(strncat(tmp, LOG_PATH, 20), "w");
    strncpy(tmp, basePath, 180);
    pUnsentFile = fopen(strncat(tmp, UNSENT_LOG_PATH, 20), "w");
    strncpy(tmp, basePath, 180);
    pSQueueFile = fopen(strncat(tmp, SQUEUE_LOG_PATH, 20), "w");
    strncpy(tmp, basePath, 180);
    pErrorFile = fopen(strncat(tmp, ERROR_LOG_PATH, 20), "w");
}



