/************************************************************************
*
*  libmgr.h
*
*  External types for the Library Manager
*
*  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/libmgr.h#1 $
*
*  Copyright 1998-2015 Citrix Systems, Inc.  
*
************************************************************************/
#ifndef _LIBMGR_H_
#define _LIBMGR_H_

#pragma once

#include "plat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LMGR_MAX_NAME 32

typedef UINT32 LMGRVERS;
typedef UINT32 LMGREVT;
typedef LMGRVERS      *PLMGRVERS;        
typedef CHAR LMGRNAME[LMGR_MAX_NAME];
typedef CHAR *PLMGRNAME;
typedef UINT32 LMGRSTATUS;
typedef PLIBPROCEDURE *PLMGRTABLE;
typedef PLMGRTABLE  *PPLMGRTABLE;

#define LMGR_EVT_INIT  1
#define LMGR_EVT_OPEN  2
#define LMGR_EVT_CLOSE 3
#define LMGR_EVT_TERM  4

/*
 * Libmgr Error Codes
 */

#define LIBMGR_STATUS_SUCCESS        0
#define LIBMGR_STATUS_INUSE          1
#define LIBMGR_STATUS_NO_MEMORY      2
#define LIBMGR_STATUS_INVALID        3
#define LIBMGR_STATUS_NOT_FOUND      4
#define LIBMGR_STATUS_NOT_SUPPORTED  5

/*
 * Function Types need to be Updated when Function declarations change
 * Function Declarations fallow below
 * Macros Parameter List Need to be Updated with Function Declarations
 * and Function Types
 */
typedef LMGRSTATUS (PWFCAPI PLMGRCONTROL)(LMGREVT);
typedef LMGRSTATUS (PWFCAPI PLMGRLOAD)(const PLMGRNAME,
                                       LMGRVERS,const PLMGRTABLE, PLMGRCONTROL);
typedef LMGRSTATUS (PWFCAPI PLMGRUNLOAD)(const PLMGRNAME);
typedef LMGRSTATUS (PWFCAPI PLMGRUSE)(const PLMGRNAME, PLMGRVERS, PPLMGRTABLE);
typedef LMGRSTATUS (PWFCAPI PLMGRRELEASE)(const PLMGRTABLE);

typedef LMGRSTATUS (PWFCAPI PLMGRQUERYTABLE)(const PLMGRNAME,
                                             PLMGRVERS pVer,
                                             PLMGRTABLE *pTable,
                                             PLMGRCONTROL *ppfnControl);

typedef LMGRSTATUS (PWFCAPI PLMGRUPDATETABLE)(const PLMGRNAME,
                                              LMGRVERS Ver,
                                              const PLMGRTABLE pTable,
                                              PLMGRCONTROL pfnControl);

#ifdef DEBUG
typedef LMGRSTATUS (PWFCAPI PLMGRLISTTABLE)(PLMGRNAME *pName, 
					    PLMGRVERS pVersion, 
					    PLMGRTABLE *pTable, 
					    PLMGRCONTROL *pfnControl);
#endif

typedef LMGRSTATUS (PWFCAPI PLMGRDUMP)();

/*
 * WARNING: do not add new function pointers to the middle of
 * this structure. Doing so will break the VCSDK. Add new
 * function pointers to the end of the structure.
 */
typedef struct _LIBMGRTABLE {
    PLMGRLOAD    pLMgrLoad;
    PLMGRUNLOAD  pLMgrUnload;
    PLMGRUSE     pLMgrUse;
    PLMGRRELEASE pLMgrRelease;
    PLMGRQUERYTABLE  pLMgrQueryTable;
    PLMGRUPDATETABLE pLMgrUpdateTable;
#ifdef DEBUG
    PLMGRLISTTABLE pLMgrListTable;
#endif
} LIBMGRTABLE, *PLIBMGRTABLE;

#define LIBMGR_GLOBAL_INIT PLIBMGRTABLE g_pLibMgrCallTable = NULL
#define LIBMGR_EXTERN_INIT  extern PLIBMGRTABLE g_pLibMgrCallTable

LIBMGR_EXTERN_INIT;


LMGRSTATUS WFCAPI LoadLibMgr(PLIBMGRTABLE*);


/*
 * Force USE_LIBMGR_CALL_TABLE for all Except the Engine
 * and Library Manager
 */
#ifdef USE_LIBMGR_CALL_TABLE
#if defined(PLATFORM_PRAGMAS) && defined(MKVERB)
#pragma MKVERB (__FILE__": LIBMGR_CALL_TABLE defined")
#endif

/*
 * If USE_LIBMGR_CALL_TABLE is defined Macros are defined to used the variable 
 * g_pLibMgrTable (USE LIBMGR_GLOBAL_INIT to define this variable) to make
 * calls to the libmgr functions.  g_pLibMgrTable must be defined before these
 * functions can be called.  
 *
 *  Macros Parameter List Need to be Updated with Function Declarations 
 *  and Function Types
 */

#define LibMgrLoad(pname,version,ptable,control) \
    (g_pLibMgrCallTable ? \
        (*(g_pLibMgrCallTable)->pLMgrLoad)(pname,version,ptable,control) : \
        LIBMGR_STATUS_NOT_SUPPORTED)
#define LibMgrUnload(pname) \
    (g_pLibMgrCallTable ? (*(g_pLibMgrCallTable)->pLMgrUnload)(pname) : \
        LIBMGR_STATUS_NOT_SUPPORTED)
#define LibMgrUse(pname,pversion,pptable) \
    (g_pLibMgrCallTable ? \
        (*(g_pLibMgrCallTable)->pLMgrUse)(pname,pversion,pptable) : \
        LIBMGR_STATUS_NOT_SUPPORTED)
#define LibMgrRelease(ptable) \
    (g_pLibMgrCallTable ? (*(g_pLibMgrCallTable)->pLMgrRelease)(ptable) : \
        LIBMGR_STATUS_NOT_SUPPORTED)
#define LibMgrQueryTable(pName, pVer, pTable, pCntrl ) \
    (g_pLibMgrCallTable ? (*(g_pLibMgrCallTable)->pLMgrQueryTable) \
        (pName, pVer, pTable, pCntrl) : \
        LIBMGR_STATUS_NOT_SUPPORTED)
#define LibMgrUpdateTable(pName, ver, pTable, pCntrl ) \
    (g_pLibMgrCallTable ? (*(g_pLibMgrCallTable)->pLMgrUpdateTable) \
        (pName, ver, pTable, pCntrl) : \
        LIBMGR_STATUS_NOT_SUPPORTED)
#ifdef DEBUG
#define LibMgrListTable(pName, ver, pTable, pCntrl ) \
    (g_pLibMgrCallTable ? (*(g_pLibMgrCallTable)->pLMgrListTable) \
        (pName, ver, pTable, pCntrl) : \
        LIBMGR_STATUS_NOT_SUPPORTED)
#endif

#else
/*
 * USE_LIBMGR_CALL_TABLE should not be defined only in the engine
 * where it is statically linked.  If statically linked just define 
 * the functions that the that are available.
 *
 *  Function Types need to be Updated when Function Declarations are Updated
 *  Function Types are above.
 *  Macros Parameter List Need to be Updated with Function Declarations
 *  and Function Types
 */

LMGRSTATUS WFCAPI LibMgrLoad(const PLMGRNAME, LMGRVERS, const PLMGRTABLE, PLMGRCONTROL);
LMGRSTATUS WFCAPI LibMgrUnload(const PLMGRNAME);
LMGRSTATUS WFCAPI LibMgrUse(const PLMGRNAME, PLMGRVERS, PPLMGRTABLE);
LMGRSTATUS WFCAPI LibMgrRelease(const PLMGRTABLE);
LMGRSTATUS WFCAPI LibMgrQueryTable(const PLMGRNAME,
                                   PLMGRVERS pver,
                                   PLMGRTABLE *pTable,
                                   PLMGRCONTROL *ppfnControl);
LMGRSTATUS WFCAPI LibMgrUpdateTable(const PLMGRNAME,
                                    LMGRVERS ver,
                                    const PLMGRTABLE pTable,
                                    PLMGRCONTROL pfnControl);
#ifdef DEBUG
LMGRSTATUS WFCAPI LibMgrListTable(PLMGRNAME *pName, 
				  PLMGRVERS pVersion, 
				  PLMGRTABLE *pTable, 
				  PLMGRCONTROL *pfnControl);
#endif

#endif /* USE_LIBMGR_CALL_TABLE */

#ifdef __cplusplus
}
#endif

#endif /* _LIBMGR_H_ */
