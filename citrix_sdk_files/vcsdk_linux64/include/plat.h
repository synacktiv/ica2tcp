/*************************************************************************
 *
 *  plat.h
 *
 *  UNIX specific platform types and conditional compilation macros.
 *
 *  Version: $Id$
 *
 *  Copyright 1999-2016 Citrix Systems, Inc.  All Rights Reserved.
 *
 ************************************************************************/
#ifndef _UNIX_PLAT_H_
#define _UNIX_PLAT_H_

#ifndef X11_SUPPORT
#define SOFT_CURSOR
#endif /*X11_SUPPORT */

#ifdef USE_THREADS
#include <pthread.h>            /* This needs to be included before anything else */
#endif /* USE_THREADS */

#define WINAPI

#define	MacOrUnix		/* Used to select common code */

#ifndef unix			/* may be defined by compiler */
#define	unix			/* to select Unix code (without Mac) */
#endif

#if 1 /* TODO get rid of this. */
#define KEEP_LEGACY_CALL_TABLES /*keep the global function table
                                  just for refrence. They will
                                  not be used in the client.*/
#endif
//#define DIRECT_VC_INPUT /* TEMP!*/
/*
 * maximum path length
 */
#if defined(HPUX) && defined (__GNUC__) && defined(__cplusplus)
/*
 * This needs to be included before <sys/time.h> (included from <sys/param.h>)
 * before gcc will compile cpp files for HP-UX.
 */
#include <sys/sigevent.h>
#endif
#include <sys/param.h>
#define _MAX_PATH MAXPATHLEN
#define PATH_MAX_SIZE _MAX_PATH
#define MAX_PATH _MAX_PATH

#include <sys/types.h>

#include "hosttype.h"
#include "basetype.h"
#include "host_os.h"

#ifndef VCSDK
#include "ctxfeatures.h"
#include "hostfeat.h"
#include "basefeat.h"
#endif

/* Support for libjpeg.so depends indirectly on the target OS. */

#if !defined(EMBED_JPEGLIB) && \
    (defined(SPEEDBROWSE) || !defined(ENABLE_PEGASUS))
#define ENABLE_DYNAMIC_LIBJPEG
#endif

#ifndef EMBEDDED_PROXY          /* Defined for minimal Linux client */
/*This #define packs proxy and SSL interfaces in a library*/
#define LIB_PROXY

#ifndef PROXYLIB   /*Not library itself. PROXYLIB comes from makefile*/
/*This is to use table of proxy library's function pointers */
#define USE_PROXY_LIB_TABLE 
#endif
#endif /* not EMBEDDED_PROXY */

#define DYNAMIC_CHANNEL_ALLOCATION

/* This gets the filename and line into log file messages. */

#ifdef __GNUC__
#define LOG_WHERE logWhere(__FILE__, __LINE__, __FUNCTION__);
#else
#if __STDC__ && __STDC_VERSION__ >= 199901L
#define LOG_WHERE logWhere(__FILE__, __LINE__, __func__);
#else
#define LOG_WHERE logWhere(__FILE__, __LINE__);
#endif
#endif

#define BYTEWIDTH 8
#define UNIXFILENAME_LENGTH	32

#ifdef X11_SUPPORT
#define HOTKEYS
#endif

#define USE_HTTP_TDO            /* HTTP code for PNA. */

int StrUpper(unsigned char *string);
#define AnsiUpper(x) StrUpper((unsigned char *)x)

/* This is to ensure that gcc can define trailing zero-length array
 * members of structs.  See twcommon.h.
 */

#ifndef ZERO_ARRAY_LENGTH
#define ZERO_ARRAY_LENGTH 0
#endif

/*
 *----------------------------------------------------------------------------
 *  UNIFICATION : Defines to get common WD compiling on Unix
 *----------------------------------------------------------------------------
 */

/* This should really be split up between all the plat*.h files but it is easier
 * having them here for convenience.
 */

#ifndef CTX_PACKED_STRUCT
#define CTX_PACKED_STRUCT
#endif

/* the definitions below tell the client to use call tables */
/* #define USE_AUBUF_CALL_TABLE */
/* #define USE_CLIB_CALL_TABLE */
#if !defined(PROXYLIB)
#define USE_CTXOS_CALL_TABLE
#endif 
/* #define USE_EVT_CALL_TABLE */
#define USE_GRAPH_CALL_TABLE
//#define USE_KBD_CALL_TABLE

#ifndef KEEP_LEGACY_CALL_TABLES
#define USE_LIBMGR_CALL_TABLE
#define USE_LOG_CALL_TABLE
#define USE_MEM_CALL_TABLE
#define USE_MEMINI_CALL_TABLE
#endif /*!KEEP_LEGACY_CALL_TABLES*/

//#define USE_MODULE_CALL_TABLE
#define USE_MOUSE_CALL_TABLE
/* #define USE_SYNC_CALL_TABLE */
/* #define USE_THRD_CALL_TABLE */
/* #define USE_TMR_CALL_TABLE */
/* #define USE_WND_CALL_TABLE */
#define USE_VID_CALL_TABLE
#define USE_SUBLST_CALL_TABLE
#define USE_FONT_CALL_TABLE
#define USE_SRCC_CALL_TABLE
#define USE_TDO_CALL_TABLE
#define USE_SCA_CALL_TABLE
#define USE_ASOCK_CALL_TABLE
/* #define USE_MMA_CALL_TABLE */
/* end of call table definitions */

/* Miscellaneous function replacements. */

#define GetSerialNumber HostRetrieveSerialNumber

#ifdef __cplusplus
extern "C"{
#endif
extern void host_sleep(IU32);

/*	Supply alternative mem* functions. The "linuxarm" build system has been modified to:

	#define memcpy	memcpy_soc
	#define memmove	memmove_soc
	#define memset	memset_soc
	 
	for all binaries except those which have been explicitly excluded.

	The purpose of this change is to take advantage of optimised versions of these functions,
	if the target device supports this.
*/
extern void *memcpy_soc (void *__restrict __dest, __const void *__restrict __src, size_t __n);
extern void *memmove_soc (void *__dest, __const void *__src, size_t __n);
extern void *memset_soc (void *__s, int __c, size_t __n);

#ifdef __cplusplus
}
#endif

extern IU32 host_getmsec(void);
#define RECORD_MOUSE_EVT  NCSconfig.lastMouseOut = host_getmsec();

typedef unsigned char *PBYTE;

#define HUGE_PTR *
#define _loadds

/*
 * Basic platform types
 */
#define TYPEDEF_NATURAL int
#define TYPEDEF_8BITS   char
#define TYPEDEF_16BITS  short
#define TYPEDEF_32BITS  int

#ifndef TYPEDEF_64BITS		/* May have been defined already, eg for Alpha */
#define TYPEDEF_64BITS	long long
#endif

#define TYPEDEF_NEAR	
#define TYPEDEF_FAR		
#define TYPEDEF_CDECL
#define TYPEDEF_PASCAL
#define TYPEDEF_FASTCALL
#define TYPEDEF_LOADDS
#define TYPEDEF_INLINE

#define TYPEDEF_CONST const

/*
 *  #pragma macros. Not used on Unix as the compiler may not
 *  expand macros within pragmas.
 *  Where #pragma pack() is required, it can be conditionally
 *  compiled by surrounding it with "#ifdef USE_PACK_PRAGMA".
 */
#if 0
#define PLATFORM_PRAGMAS
#define ENG_PACKING_ENABLE  pack(1)
#define ENG_PACKING_RESTORE pack()

#define ICA_PACKING_ENABLE  pack(1)
#define ICA_PACKING_RESTORE pack()

#endif

#define USE_PACK_PRAGMA

/*
 * stdint.h to get types for void pointers
 * these are currently used by LogAssert, which needs a type
 * which can work on both 64 bit and 32 bit builds
 */
#include <stdint.h>

/*
 * Basic platform types
 * Note X_BOOLBYTE_TYPES is defined to get
 * certain csm files to build where there
 * is a conflict with X11 types.
 */
typedef signed TYPEDEF_NATURAL INT, *PINT;
#ifdef X_BOOLBYTE_TYPES
#include <X11/Xmd.h>
#else
typedef signed TYPEDEF_8BITS   INT8;
#endif
typedef signed TYPEDEF_8BITS   TYPEDEF_NEAR *PINT8;
typedef signed TYPEDEF_8BITS   TYPEDEF_FAR  *LPINT8;

#ifndef X_BOOLBYTE_TYPES
typedef signed TYPEDEF_16BITS  INT16;
#endif
typedef signed TYPEDEF_16BITS  TYPEDEF_NEAR *PINT16;
typedef signed TYPEDEF_16BITS  TYPEDEF_FAR  *LPINT16;

#ifndef X_BOOLBYTE_TYPES
typedef signed TYPEDEF_32BITS  INT32;
#endif
typedef signed TYPEDEF_32BITS  TYPEDEF_NEAR *PINT32;
typedef signed TYPEDEF_32BITS  TYPEDEF_FAR  *LPINT32;

typedef UINT  *PUINT;
typedef unsigned TYPEDEF_8BITS    UINT8;
typedef unsigned TYPEDEF_8BITS    *PUINT8;
typedef PUINT8 LPUINT8;
typedef unsigned TYPEDEF_16BITS   UINT16;
typedef unsigned TYPEDEF_16BITS   *PUINT16;
typedef PUINT16 LPUINT16;
typedef unsigned TYPEDEF_32BITS   UINT32;
typedef unsigned TYPEDEF_32BITS   *PUINT32;
typedef PUINT32 LPUINT32;

#ifndef X_BOOLBYTE_TYPES
typedef TYPEDEF_64BITS   INT64;
#endif
typedef TYPEDEF_64BITS   *PINT64;
typedef unsigned TYPEDEF_64BITS    UINT64;
typedef unsigned TYPEDEF_64BITS    *PUINT64;

typedef TYPEDEF_8BITS 	CHAR;
typedef TYPEDEF_16BITS  SHORT;

#ifdef X_BOOLBYTE_TYPES
#define LEGACY_TYPES_DEFINED
typedef void *PVOID;
typedef unsigned long ULONG,*PULONG;
typedef unsigned char UCHAR, *PUCHAR;
typedef CHAR *PCHAR;
typedef unsigned short USHORT;
typedef UCHAR BOOLEAN;
#define BOOLEAN_DEFINED
typedef BOOLEAN TYPEDEF_FAR *PBOOLEAN;

#endif

typedef UINT16		WORD;
typedef WORD *		LPWORD;

typedef UINT32		DWORD;
typedef DWORD *		LPDWORD;
typedef DWORD *		PDWORD;
typedef DWORD		WPARAM;


typedef INT32		HRESULT;
#ifdef TYPEDEF_64BITS
typedef TYPEDEF_64BITS LARGE_INTEGER;
#else
typedef TYPEDEF_32BITS LARGE_INTEGER;
#endif
// Selection of HRESULT values referenced by the client
#define S_OK					((HRESULT)0L)
#define S_FALSE					((HRESULT)1L)

#define SUCCEEDED(hr)			(((HRESULT)(hr)) >= 0)
#define FAILED(hr)				(((HRESULT)(hr)) < 0)

#define E_FAIL					((HRESULT)0x80004005L)
#define E_UNEXPECTED			((HRESULT)0x8000FFFFL)
#define E_INVALIDARG 			((HRESULT)0x80070057L)
#define E_OUTOFMEMORY			((HRESULT)0x8007000EL)
#define E_ABORT					((HRESULT)0x80004004L)
#define E_NOTIMPL				((HRESULT)0x80004001L)
#define E_ACCESSDENIED			((HRESULT)0x80070005L)


/* VP character for basic VP strings */
typedef CHAR TYPEDEF_STRINGCHAR;


#include <platcall.h>
#include <platcomm.h>

/* Maximum size of something got from the environment */

#define MAXENV			1024

/* These are used in dll.c. */

#define DIR_SEPARATOR '/'
#define NON_DIR_SEPARATOR '\\'

/* Winstation driver polling interval: this drives the client code. */

#define WD_POLL_INTERVAL        100		/* in milliseconds */

#include "platfix.h"

/*
 *----------------------------------------------------------------------------
 *  UNIFICATION : End                     
 *----------------------------------------------------------------------------
 */

/*
 * The directory in which the registry ini file is created and the relative
 * location of its template file. The directory path may contain an
 * environment variable to be expanded.
 */

#define INI_REG_DIR     "$HOME/.ICAClient"
#define INI_REG_FILE    "reg.ini"
#define INI_REG_DEFAULT "config/" INI_REG_FILE

/* the names of the NCS and CSM executables */
#define CLIENT               "wfica"
#define CLIENT_MANAGER       "wfcmgr"

/* for VDMM (RAVE) */
#define _HRESULT_TYPEDEF_(_sc) _sc
typedef double DOUBLE;



#ifdef NCS

#include "language.h"

extern void RemoveSplashResources(void);
extern char *ServerName();

#endif

/* Macro to allow main() to invoke code to record and drop effective group 
 * permissions.
 */
#ifdef NCS
#define SecurityInitialisation() InitialiseIdentity()

void InitialiseIdentity(void);
#endif /* NCS */

/* Cpr 145279
This #define is to keep track of locks on a file*/
#define TRACK_FILE_LOCKS

/*This is to add support for windows type sharing modes*/
#define SHARING_MODE_SUPPORT


/* Global directory.  This is a collection of pointers to various structures
 * of global significance in the client, where access was
 * previously complicated.  They contain the the state of various internal
 * modules, so should be treated as read-only.
 */

struct tagTW2_INSTANCE;
struct _WD;
struct _ICAENG_INITSESSIONDATA;

extern struct _Global_pointers {
    struct _DLLLINK                *wd_link;
    struct _WD                     *wd_data;
    struct tagTW2_INSTANCE         *tw2_data;
    struct _TWI_LOCAL_CONTEXT      *twi_data;
    struct _TD_PD                  *td_data;
    struct _ICAENG_INITSESSIONDATA *desired_session;
    BOOLEAN                         gateway_supports_msi;
} Global_pointers;

/* Access to the above should be through this macro, as we may want to
 * make that a thread-local variable in a multi-session client, or similar.
 */

#define SGDP (&Global_pointers)

/* These are here so that calls can be put almost anywhere without
 * adding a header, for printf-style debugging in environments lacking
 * a working printf().
 */

extern void Log_error(char *format, ...);
extern void Log_warning(char *format, ...);
extern void Log_report(char *format, ...);

/* Fatal dialogue box when an ICA protocol error is found. */

extern void Proto_error(void);
#endif /* _UNIX_PLAT_H_ */
