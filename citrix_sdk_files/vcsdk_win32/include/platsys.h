/*************************************************************************
*
*  win32\platsys.h
*
*  Basic platform abstractions
*
*  Copyright 1998-2015 Citrix Systems, Inc. 
*
*************************************************************************/

#ifndef _WIN32_PLATSYS_H_
#define _WIN32_PLATSYS_H_

/*
 * Platform specfic prototypes
 */
extern  BOOL IsSystemWinNT( );

/*
 * Platform specific macros
 */

#define CtxCharUpper(p)                 CharUpperA(p)

/*
 * For WIN32, we will wait for any incoming network data or any message
 * in the queue. This is to optimise the wait loop. Otherwise, the desktop
 * CPU utilization will go to 100%. hSocketEvent is a global variable defined
 * in wengine.c. If it is NULL, the underlying TD supports the asynchronous
 * notification.
 */
#ifdef WIN32
#define CtxYield()  \
    if (hSocketEvent) { \
        MsgWaitForMultipleObjects(1, &hSocketEvent, FALSE, 25, QS_ALLINPUT); \
        ResetEvent(hSocketEvent); \
    }   \
    else {  \
        SleepEx(25, TRUE);  \
    }
#else
#define CtxYield()                      SleepEx(25, TRUE);
#endif  /* WIN32 */

#define HCTXGLOBAL                      HGLOBAL
#endif /* _WIN32_PLATSYS_H_ */
