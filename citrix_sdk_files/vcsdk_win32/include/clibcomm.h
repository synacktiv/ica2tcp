/*************************************************************************
*
* clibcomm.h
*
* Header file for C runtime library routines common to all platforms
*  
* Copyright 1998-2015 Citrix Systems, Inc. 
*
*************************************************************************/

#ifndef _CLIBCOMM_H_
#define _CLIBCOMM_H_

#pragma once

#ifndef NO_CLIB_CALL_TABLE	/* Suppress table definition in Unix build. */

/*
 *  Define types that are used as parameters in the function pointer
 *  types
 */
typedef void (PWFCAPI PFNPANIC)( void );
#ifndef macintosh
typedef PCHAR FAR * PPCHAR;
#endif

/*
 * hook procedure structure
 */
typedef struct _FNHOOK {
   void * pProcedure;
   struct _FNHOOK * pNext;
} FNHOOK, TYPEDEF_FAR * PFNHOOK, TYPEDEF_FAR * TYPEDEF_FAR *PPFNHOOK;



/*
 *  Type define function pointers
 */
typedef PCHAR   (PWFCAPI PCLIBCURDIR)   ( USHORT, USHORT );
typedef INT     (PWFCAPI PCLIBACCESS)   ( const PCHAR, INT);
typedef INT     (PWFCAPI PCLIBOPEN)     ( PCHAR, INT);
typedef INT     (PWFCAPI PCLIBREAD)     ( INT, PCHAR, UINT);
typedef INT     (PWFCAPI PCLIBWRITE)    ( INT, PCHAR, UINT);
typedef INT     (PWFCAPI PCLIBCLOSE)    ( INT);
typedef FILE FAR *(PWFCAPI PCLIBFOPEN)  ( const PCHAR, const PCHAR);
typedef INT     (PWFCAPI PCLIBFREAD)    ( LPVOID, UINT, UINT, FILE FAR * );
typedef INT     (PWFCAPI PCLIBFPRINTF)  ( FILE FAR *, const PCHAR, ... );
typedef INT     (PWFCAPI PCLIBFCLOSE)   ( FILE FAR * );
typedef INT     (PWFCAPI PCLIBPUTCH)    ( int );
typedef INT     (PWFCAPI PCLIBPRINTF)   ( const PCHAR, ...);
typedef INT     (PWFCAPI PCLIBSPRINTF)  ( PCHAR, const PCHAR, ...);
typedef LPVOID  (PWFCAPI PCLIBMALLOC)   ( UINT);
typedef LPVOID  (PWFCAPI PCLIBMALLOCL)  ( ULONG);
typedef LPVOID  (PWFCAPI PCLIBREALLOC)  ( LPVOID, UINT);
typedef LPVOID  (PWFCAPI PCLIBREALLOCL) ( LPVOID, ULONG );
typedef PCHAR   (PWFCAPI PCLIBSTRDUP)   ( PCHAR );
typedef UINT    (PWFCAPI PCLIB_MSIZE)   ( LPVOID );
typedef INT     (PWFCAPI PCLIBFREE)     ( LPVOID);
typedef PCHAR   (PWFCAPI PCLIBGETCWD)   ( PCHAR, INT);
typedef INT     (PWFCAPI PCLIBCHDIR)    ( PCHAR);
typedef long    (PWFCAPI PCLIBLSEEK)    ( int fd, long offset, int origin );
typedef void    (PWFCAPI PCLIBREBOOT)   ( void );
typedef int     (PWFCAPI PCLIBALLOCHOOK)( PPFNHOOK, LPVOID );
typedef int     (PWFCAPI PCLIBDEALLOCHOOK)  ( PPFNHOOK, LPVOID );
typedef int     (PWFCAPI PCLIB_DOS_OPEN)    ( PCHAR, UINT, INT * );
typedef int     (PWFCAPI PCLIB_DOS_CREAT)   ( PCHAR, UINT, INT * );
typedef int     (PWFCAPI PCLIB_DOS_CREATNEW)( PCHAR, UINT, INT * );
typedef int     (PWFCAPI PCLIB_DOS_CLOSE)   ( INT );
typedef int     (PWFCAPI PCLIB_DOS_DELETE)  ( PCHAR );
typedef int     (PWFCAPI PCLIB_DOS_READ)    ( INT, LPVOID, UINT, UINT * );
typedef int     (PWFCAPI PCLIB_DOS_WRITE)   ( INT, LPVOID, UINT, UINT * );
typedef int     (PWFCAPI PCLIB_DOS_FINDFIRST)   ( PCHAR, UINT, LPVOID );
typedef int     (PWFCAPI PCLIB_DOS_FINDNEXT)    ( LPVOID );
typedef int     (PWFCAPI PCLIB_DOS_GETFILEATTR) ( PCHAR, UINT * );
typedef int     (PWFCAPI PCLIB_DOS_SETFILEATTR) ( PCHAR, UINT );
typedef int     (PWFCAPI PCLIB_DOS_GETFTIME)    ( INT, UINT *, UINT * );
typedef int     (PWFCAPI PCLIB_DOS_SETFTIME)    ( INT, UINT, UINT );
typedef int     (PWFCAPI PCLIB_DOS_GETTIME)     ( LPVOID );
typedef int     (PWFCAPI PCLIB_DOS_GETDATE)     ( LPVOID );
typedef int     (PWFCAPI PCLIB_DOS_GETDISKFREE) ( UINT, LPVOID );
typedef int     (PWFCAPI PCLIB_DOS_GETDRIVE)    ( UINT * );
typedef int     (PWFCAPI PCLIB_DOSEXTERR)       ( LPVOID );
typedef unsigned short * (PWFCAPI PCLIB_DOS_GETHARDERR)( );
typedef VOID    (PWFCAPI PCLIBDISPLAYHEAP)      ( VOID );
typedef int     (PWFCAPI PCLIBFLUSH)            ( int fd );
typedef void    (PWFCAPI PCLIBQUERYHEAP)        ( PULONG, PULONG, PULONG );
typedef long    (PWFCAPI PCLIBFSEEK)            ( FILE FAR *, long, int );
typedef PCHAR   (PWFCAPI PCLIBFGETS)            ( PVOID, UINT, FILE FAR * );
typedef void    (PWFCAPI PCLIBFUNGETS)          ( FILE FAR * );
typedef INT     (PWFCAPI PCLIB_DOS_GETMEDIAID)  ( BYTE, PULONG );
typedef BYTE    (PWFCAPI PCLIB_DOS_BOOTDRIVE)   ( );
typedef long    (PWFCAPI PCLIBFTELL)            ( FILE FAR * );
typedef INT     (PWFCAPI PCLIBRENAME)           ( PCHAR, PCHAR);
typedef INT     (PWFCAPI PCLIBREMOVE)           ( const PCHAR);
typedef INT     (PWFCAPI PCLIBRMDIR)            ( const PCHAR);
typedef INT     (PWFCAPI PCLIBMKDIR)            ( const PCHAR);
typedef USHORT  (PWFCAPI PSPRF)                 ( PCHAR, PPCHAR, ... );
typedef int     (PWFCAPI PSETARGV)              ( USHORT _psp, int *, char *** );
typedef void    (PWFCAPI PFREEARGV)             ( char **, int );
typedef void    (PWFCAPI PREGISTERPANIC)        ( PFNPANIC pfnPanic );
typedef int     (PWFCAPI PCLIBGETLASTERRNO)     ();

/*
 *  Define Structure that contains pointers to function
 *  of the types define above
 */
typedef struct _CLIBTABLE
{
    PCLIBCURDIR             pcurdir;
    PCLIBACCESS             paccess;
    PCLIBOPEN               popen;
    PCLIBREAD               pread;
    PCLIBWRITE              pwrite;
    PCLIBCLOSE              pclose;
    PCLIBFOPEN              pfopen;
    PCLIBFREAD              pfread;
    PCLIBFPRINTF            pfprintf;
    PCLIBFCLOSE             pfclose;
    PCLIBPUTCH              pputch;
    PCLIBPRINTF             pprintf;
    PCLIBSPRINTF            psprintf;
    PCLIBMALLOC             pmalloc;
    PCLIBREALLOC            prealloc;
    PCLIBSTRDUP             pstrdup;
    PCLIB_MSIZE             p_msize;
    PCLIBFREE               pfree;
    PCLIBGETCWD             pgetcwd;
    PCLIBCHDIR              pchdir;
    PCLIBLSEEK              plseek;
    PCLIBREBOOT             preboot;
    PCLIBALLOCHOOK          pAllocHook;
    PCLIBDEALLOCHOOK        pDeallocHook;
    PCLIB_DOS_OPEN          p_dos_open;
    PCLIB_DOS_CREAT         p_dos_creat;
    PCLIB_DOS_CREATNEW      p_dos_creatnew;
    PCLIB_DOS_CLOSE         p_dos_close;
    PCLIB_DOS_DELETE        p_dos_delete;
    PCLIB_DOS_READ          p_dos_read;
    PCLIB_DOS_WRITE         p_dos_write;
    PCLIB_DOS_FINDFIRST     p_dos_findfirst;
    PCLIB_DOS_FINDNEXT      p_dos_findnext;
    PCLIB_DOS_GETFILEATTR   p_dos_getfileattr;
    PCLIB_DOS_SETFILEATTR   p_dos_setfileattr;
    PCLIB_DOS_GETFTIME      p_dos_getftime;
    PCLIB_DOS_SETFTIME      p_dos_setftime;
    PCLIB_DOS_GETTIME       p_dos_gettime;
    PCLIB_DOS_GETDATE       p_dos_getdate;
    PCLIB_DOS_GETDISKFREE   p_dos_getdiskfree;
    PCLIB_DOS_GETDRIVE      p_dos_getdrive;
    PCLIB_DOSEXTERR         pdosexterr;
    PCLIB_DOS_GETHARDERR    p_dos_getharderr;
    PCLIBMALLOCL            pmallocL;
    PCLIBREALLOCL           preallocL;
    PCLIBDISPLAYHEAP        pDisplayHeap;
    PCLIBFLUSH              pflush;
    PCLIBQUERYHEAP          pQueryHeap;
    PCLIBFSEEK              pfseek;
    PCLIBFGETS              pfgets;
    PCLIBFUNGETS            pfungets;
    PCLIB_DOS_GETMEDIAID    p_dos_getmediaid;
    PCLIB_DOS_BOOTDRIVE     p_dos_bootdrive;
    PCLIBFTELL              pftell;
    PCLIBRENAME             prename;
    PCLIBREMOVE             premove;
    PCLIBRMDIR              prmdir;
    PCLIBMKDIR              pmkdir;
    PSPRF                   p_sprf;
    PSETARGV                psetargv;
    PFREEARGV               pfreearg;
    PREGISTERPANIC          pRegisterPanic;
    PCLIBGETLASTERRNO       pGetLastErrno;
}CLIBTABLE, FAR *PCLIBTABLE, FAR * FAR *PPCLIBTABLE;


/*
 *  Function Prototypes
 */
int WFCAPI ClibLoad( PPCLIBTABLE );

#endif  /* NO_CLIB_CALL_TABLE */

/*
 *  Define unique string to Identify Table pointer with the 
 *  LibMgr
 */
#define CLIB_STRING "CTXCLIB"

/*
 *  Define the set of macros to use the CLIBTALBE structure
 *  if USE_CLIB_CALL_TABLE is defined otherwise set 
 *  macros to nothing.  This allows the code to build correctly
 *  without ifdefs in the code
 */

#ifdef USE_CLIB_CALL_TABLE
#pragma MKVERB (__FILE__": CLIB_CALL_TABLE defined")
/*
 *  Macros to assist in defining Table structure  
 *  USE these macros to force the correct name    
 */

#define CLIB_EXTERN_INIT extern PCLIBTABLE g_pClibCallTable 
#define CLIB_GLOBAL_INIT PCLIBTABLE g_pClibCallTable =NULL

/*
 *   Macros assist in initializing global Table pointer value
 */
#define ClibRunTimeInit(pLibMgr, pVer) \
  ((*pLibMgr->pLMgrUse)(CLIB_STRING,pVer,(PPLMGRTABLE)&g_pClibCallTable))

#define ClibRunTimeRelease(pLibMgr) \
  ((*pLibMgr->pLMgrRelease)((PLMGTABLE)g_pClibCallTable))

CLIB_EXTERN_INIT;

#else /* !USE_CLIB_CALL_TABLE */

/*  
 *  Define the Macros to Nothing because they are not using 
 *  the Call table.
 */
#define CLIB_GLOBAL_INIT 
#define CLIB_EXTERN_INIT 

#define ClibRunTimeInit(pLibMgr, pVer) 
#define ClibRunTimeRelease(pLibMgr)

#endif /* !USE_CLIB_CALL_TABLE */

/*
 *  Time routines
 */


long _CAPI Getmsec( VOID );      /* get time in 1/1000 seconds */

#endif /* _CLIBCOMM_H_ */
