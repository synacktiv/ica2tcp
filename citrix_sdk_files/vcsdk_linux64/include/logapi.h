/********************************************************************
*
*  LOGAPI.H
*
*  Header file for Logging and Tracing
*
*  $Id$
*
*  Copyright 1990-2020 Citrix Systems, Inc. All Rights Reserved.
*
*********************************************************************/

#ifndef __LOGAPI_H__
#define __LOGAPI_H__

#if defined(PLATFORM_PRAGMAS) && defined(MKDBG)
#pragma MKDBG("FIX: engtypes.h included by logapi.h")
#endif

#include "engtypes.h"
#include "ctxdebug.h"

/* Macro to ensure null string pointers are not passed to TRACE on systems
 * that do not tolerate them as the argument for a "%s" format.
 */

#ifndef SAFE_STRING
#define SAFE_STRING(x) ((x) ? (char*) (x) : "")
#endif

#define BOOL_STRING(x) ((x) ? "TRUE" : "FALSE")
#define BOOL_CHAR(x)   ((x) ? 'T' : 'F')

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Logging and Tracing flag definitions
 */

#include "logflags.h"

#include <stdarg.h>

/*
 *  Logging structures
 */

typedef struct _LOGOPEN {
	ULONG LogFlags;
	ULONG LogClass;
	ULONG LogEnable;
    ULONG LogBufferSize;
	FILEPATH LogFile;
} LOGOPEN, FAR *PLOGOPEN;

/*
 *  Logging functions
 */

//int LogLoad(void);
#ifdef __GNUC__
void logWhere(const char *, int, const char *);
#else
void logWhere(const char *, int);
#endif
BOOLEAN LogIsTraced(ULONG LogClass, ULONG LogEnable);
void  logPrintf(ULONG LogClass, ULONG LogEnable, const char * pFormat, va_list arg_marker);

#define LOG_STRING "CTXLOG"

int  LogClose(void);
void LogPrintf(ULONG LogClass, ULONG LogEnable, const char * pFormat, ...);
void LogBuffer(ULONG LogClass, ULONG LogEnable,
               const LPVOID pBuffer, ULONG ByteCount);
#ifdef __GNUC__
void LogAssert(const char*, const char*, int, const char*, intptr_t);
#else
void LogAssert(PCHAR, PCHAR, int, intptr_t);
#endif

#define LOG_EXTERN_INIT

#define LogRunTimeInit(pLibMgr, pVer)
#define LogRunTimeRelease(pLibMgr)
#define LogSetCallTable(pLogCallTable)

// Syslog definitions
void logPrintf_Retail(ULONG LogClass, ULONG LogEnable, const char * pFormat, va_list arg_marker);
void RetailLogPrintf(ULONG LogClass, ULONG LogEnable, const char * pFormat, ...); /*Function for Logging to syslog for Retail Build */

// Redirect to new logging Api
void CtxTraceRedir(ULONG LogClass, ULONG LogEnable, const char * pFormat, ...);

// The message table defined in logmesages.c
extern char *LogMessages[];

// Defines for each message, that equate into an index into the table
#define LOGMESSAGE_DLLLOADFAILED LogMessages[0]

#ifndef ANDROID
void ncsDebugLogExit(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LOGAPI_H__ */
