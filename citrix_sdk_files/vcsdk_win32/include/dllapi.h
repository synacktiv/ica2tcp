/***************************************************************************
*
*  DLLAPI.H
*
*  This module contains client DLL access definitions
*
*  Copyright 1998-2015 Citrix Systems, Inc.  
*
****************************************************************************/

#ifndef __DLLAPI_H__
#define __DLLAPI_H__

#pragma once

#include <libmgr.h>

#if defined(PLATFORM_PRAGMAS) && defined(MKDBG)
#if defined(_MINIDLL_H_)
#pragma MKDBG("MINIDLL is included prior to DLLAPI")
#endif
#endif /* PLATFORM_PRAGMAS && MKDBG */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EMBEDDLL FAR *PEMBEDDLL;
/*
 *  DLL link structure
 */

#define MODULE_LENGTH 13

typedef struct _DLLLINK {
/* NOTE: 1st six elements must be same as MINIDLL */
    USHORT Segment;             /* starting seg of mem allocated for dll (aligned) */
    USHORT DllSize;             /* actual size of dll in paragraphs */
    USHORT ProcCount;           /* number of procedures in call table */
    PVOID pProcedures;          /* pointer to procedure call table */
    PVOID pData;                /* pointer to dll data structure */
    PUCHAR pMemory;             /* pointer to malloced memory (not aligned) */
    BYTE ModuleName[MODULE_LENGTH];        /* client module name (8.3) */
    LPVOID pLibMgrCallTable;    /* Pointer to LibMgrTable (input) */
    USHORT ModuleDate;          /* module date in dos format */
    USHORT ModuleTime;          /* module time in dos format */
    ULONG ModuleSize;           /* module file size in bytes */
    struct _DLLLINK * pNext;    /* pointer to next DLLLINK structure */
    ULONG DllFlags;             /* DLL flags (embed..) */
    /* Everything after here is not included for the ModuleEnum call. */

    HND		LibraryHandle;

} DLLLINK, * PDLLLINK;

typedef int (PWFCAPI PMAINLOAD)( PDLLLINK );
#define ENUM_DLLLINK_SIZE (sizeof(DLLLINK))  

/*
 * Dll flags definition (32 of them possible)
 */
#define DLL_FLAGS_EMBED    1
#define DLL_FLAGS_BUSY     2

typedef int (PWFCAPI PLOADPROCEDURE)( PDLLLINK, USHORT );
typedef int (PWFCAPI PDLLPROCEDURE)( PVOID, PVOID, PUINT16 );
#define MODULExLOAD            0
#define MODULExUNLOAD          1
#define MODULExENUM            2
#define MODULExCALL            3

 

typedef int (PWFCAPI PMODULELOAD)( char *, PDLLLINK );
typedef int (PWFCAPI PMODULEUNLOAD)( PDLLLINK );
typedef int (PWFCAPI PMODULEENUM)( LPBYTE, USHORT, PUSHORT );
typedef int (PWFCAPI PMODULECALL)( PDLLLINK, USHORT, PVOID, PUINT16 );

/* 
 * WARNING: do not add new function pointers to the middle of 
 * this structure. Doing so will break the VCSDK. Add new 
 * function pointers to the end of the structure.
 */
typedef struct _MODULETABLE
{
    PMODULELOAD     pModuleLoad;
    PMODULEUNLOAD   pModuleUnload;
    PMODULEENUM     pModuleEnum;
    PMODULECALL     pModuleCall;
}MODULETABLE,  * PMODULETABLE,  *  * PPMODULETABLE;

int WFCAPI ModuleInit(  PDLLLINK, PPMODULETABLE,PMAINLOAD,PEMBEDDLL );
int WFCAPI ModuleUninit( );


#define MODULE_STRING "CTXMODULE"

#ifdef USE_MODULE_CALL_TABLE
#if defined(PLATFORM_PRAGMAS) && defined(MKVERB)
#pragma MKVERB (__FILE__": MODULE_CALL_TABLE defined")
#endif

/*
 * Compiling the library itself
 */

#define MODULE_GLOBAL_INIT PMODULETABLE g_pModuleCallTable =NULL
#define MODULE_EXTERN_INIT extern PMODULETABLE g_pModuleCallTable

#define ModuleRunTimeInit(pLibMgr, pVer) \
    ((*pLibMgr->pLMgrUse)(MODULE_STRING,pVer,(PPLMGRTABLE)&g_pModuleCallTable))
#define ModuleRunTimeRelease(pLibMgr) \
    ((*pLibMgr->pLMgrRelease)((PLMGTABLE)g_pModuleCallTable))

MODULE_EXTERN_INIT;

/*
 * Note: These function typedefs must be maintained in sync with the
 *       above function prototypes
 */


#define ModuleLoad     (*(g_pModuleCallTable)->pModuleLoad)
#define ModuleUnload   (*(g_pModuleCallTable)->pModuleUnload)
#define ModuleEnum     (*(g_pModuleCallTable)->pModuleEnum)
#define ModuleCall     (*(g_pModuleCallTable)->pModuleCall)

#else /* !USE_MODULE_CALL_TABLE */

/*
 * Note: These function prototypes must be maintained in sync with the
 *       function typedefs below
 */
int WFCAPI ModuleLoad( char *, PDLLLINK );
int WFCAPI ModuleUnload( PDLLLINK );
int WFCAPI ModuleEnum( LPBYTE, USHORT, PUSHORT );
int WFCAPI ModuleCall( PDLLLINK, USHORT, PVOID, PUINT16 );

#define MODULE_GLOBAL_INIT
#define MODULE_EXTERN_INIT

#define ModuleRunTimeInit(pLibMgr, pVer)
#define ModuleRunTimeRelease(pLibMgr)

#endif /* !USE_MODULE_CALL_TABLE */

/*
 *  ExeOpen structure
 */
typedef struct _EXEOPEN {
    PVOID pIniSection;
} EXEOPEN, * PEXEOPEN;


/*=============================================================================
 ==   Common dll entry points for all dlls
 ============================================================================*/
 

#define DLLxLOAD                 0
#define DLLxUNLOAD               1
#define DLLxOPEN                 2
#define DLLxCLOSE                3
#define DLLxINFO                 4
#define DLLxPOLL                 5

/*
 *  DLLxCLOSE structure
 */
typedef struct _DLLCLOSE {
    int NotUsed;
} DLLCLOSE, * PDLLCLOSE;

/*
 *  DLLxINFO structure
 */
typedef struct _DLLINFO {
    LPBYTE pBuffer;
    USHORT ByteCount;
} DLLINFO, * PDLLINFO;

/*
 *  DLLxPOLL structure
 */
typedef struct _DLLPOLL {
    ULONG CurrentTime;          /* current time in msec */
} DLLPOLL, * PDLLPOLL;

// DLL_HPC_POLL structure (HPC API Version 1)
// This structure is passed to the VD DllPoll function when the VD identifies itself
// as a VD coded to HPC API Version 1, via the Wd_Set(WDAPI_USEHPCAPIVERSION) call.
// Otherwise, the DLLPOLL structure is passed.

typedef struct _DLL_HPC_POLL
{
	ULONG ulCurrentTime;				// the time of the poll.
	INT nFunction;						// function code.  See HPCPOLL_FUNCTION_*, below.
	INT nSubFunction;					// subfunction code.  These values are dependent on the HPCPOLL_FUNCTION_* code.
	LPVOID pUserData;					// pointer to VD user data passed via Hpc_Comm_ApiSendData or WDxSETINFORMATION(WdRequestPoll)
} DLL_HPC_POLL, *PDLL_HPC_POLL;

// HPCPOLL_FUNCTION:  This enum is used for the nFunction field of the DLL_HPC_POLL structure
// which is passed to to the VD DriverPoll function by the HPC engine.  The nCode variable
// will be se to zero for each of these function types.
// HPC API Version 1

typedef enum _HPCPOLL_FUNCTION
{
	HPCPOLL_FUNCTION_POLL,				// This is a standard DriverPoll call.  
										// The VD is being polled to send any data that it has available.
										// nCode: zero.
										// pUserData: NULL
    HPCPOLL_FUNCTION_NOTIFY,			// An Hpc_Comm_ApiSendData notification.
										// nCode: Can differentiate between notification statuses.
										// pUserData: Value passed on Hpc_Comm_ApiSendData.
    HPCPOLL_FUNCTION_REQUESTEDPOLL		// This is the response to the VD's Wd_Set(WDAPI_REQUESTPOLL).
										// nCode: 0
										// pUserData: Value passed on Wd_Set(WDAPI_REQUESTPOLL).
} HPCPOLL_FUNCTION;

typedef INT (PWFCAPI PEMBEDLOAD)( PDLLLINK pLink );

typedef struct _EMBEDDLL {
  char      *DllName;                  /* name of DLL */
  PEMBEDLOAD DllLoad;                  /* DLL load function */
  PVOID      pfnMain;                  /* DLL LIBMAIN or DLLMain entry */
} EMBEDDLL;


#ifdef __cplusplus
}
#endif

#endif /* __DLLAPI_H__ */
