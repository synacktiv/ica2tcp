/*************************************************************************
 *
 *  win32\plat.h
 *
 *  WIN32-specific platform types.
 *
 *  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/win32/plat.h#1 $
 *
 *  Copyright 1999-2015 Citrix Systems, Inc. 
 *
 ************************************************************************/

#ifndef _WIN32_PLAT_H_
#define _WIN32_PLAT_H_

#include <ctxfeatures.h>

/* The following define allows the inclusion of a dll called icagraph.dll
 * which allows 3rd party implemenations of the GDI calls, as well as an
 * aid to profiling.
 * see /test/ICAGraph/ICAGraph.txt for details and example dll code.
 */
//#define EXPORT_GDI

/*
 * Disable some warnings that are frequent in windows code
 */
#pragma warning(disable : 4209 4201 4214 4115 4514)

#ifndef _MFC_VER

/* 
 * Prevent the include of winsock.  Let the source determine the 
 * version it needs.  After windows get included undefine guards.
 */

#define _MSWSOCK_
#define _WINSOCKAPI_
#define _WINSOCK2API_
#include <windows.h>
#undef  _WINSOCKAPI_
#undef  _WINSOCK2API_
#undef  _MSWSOCK_
#define LEGACY_TYPES_DEFINED
#endif

/*
 * Re-enable the warnings for client code
 */
#pragma warning(default : 4209 4201 4115)

#define TW2_SCALE_TIMERS    /* Over-scrolling control */

/*
 * Basic platform types
 */
#define TYPEDEF_NATURAL int
#define TYPEDEF_8BITS   char
#define TYPEDEF_16BITS  short
#define TYPEDEF_32BITS  long

#define TYPEDEF_NEAR
#define TYPEDEF_FAR
#define TYPEDEF_CDECL     __cdecl
#define TYPEDEF_PASCAL    __stdcall
#define TYPEDEF_FASTCALL  __fastcall
#define TYPEDEF_LOADDS
#define TYPEDEF_INLINE    __inline

#define TYPEDEF_CONST   const

/*
 * maximum path length
 */
#define PATH_MAX_SIZE   260

/* Windows has an idiosyncratic conversion format for
 * unsigned 64-bit integers in printf() strings.
 */

#define UINT64_FORMAT "%I64u"

/*
 * Define linkage type for various components
 */
#define USE_AUBUF_CALL_TABLE
#define USE_AUD_CALL_TABLE
#define USE_BINI_CALL_TABLE
#define USE_CLIB_CALL_TABLE
#define USE_CTXOS_CALL_TABLE
#define USE_ENG_CALL_TABLE
#define USE_EVT_CALL_TABLE
#define USE_GRAPH_CALL_TABLE
#define USE_MSGRAPH_CALL_TABLE
#define USE_HND_CALL_TABLE
#define USE_INI_CALL_TABLE
#define USE_KBD_CALL_TABLE
#define USE_LIBMGR_CALL_TABLE
#define USE_LOG_CALL_TABLE
#define USE_MEM_CALL_TABLE
#define USE_MEMINI_CALL_TABLE
#define USE_MODULE_CALL_TABLE
#define USE_MOUSE_CALL_TABLE
#define USE_REDUCER_CALL_TABLE
#define USE_SYNC_CALL_TABLE
#define USE_THRD_CALL_TABLE
#define USE_TMR_CALL_TABLE
#define USE_VIO_CALL_TABLE
#define USE_VP_CALL_TABLE
#define USE_WND_CALL_TABLE
#define USE_SUBLST_CALL_TABLE
#define USE_SRCC_CALL_TABLE
#define USE_FILE_CALL_TABLE
#define USE_PATH_CALL_TABLE
#define USE_SCA_CALL_TABLE
#define USE_ASOCK_CALL_TABLE
#define USE_MMA_CALL_TABLE
#define USE_DVC_CALL_TABLE

/*
 *  #pragma macros
 */
#define PLATFORM_PRAGMAS
#define ENG_PACKING_ENABLE  pack(8)
#define ENG_PACKING_RESTORE pack()

#define ICA_PACKING_ENABLE  pack(1)
#define ICA_PACKING_RESTORE pack()

#include <platcall.h>

/*
 * Define the basic INT types that conflict with some platforms
 */

/* typedef signed TYPEDEF_NATURAL INT; Defined by platform */
typedef signed TYPEDEF_8BITS   INT8;
typedef signed TYPEDEF_8BITS   TYPEDEF_NEAR *PINT8;
typedef signed TYPEDEF_8BITS   TYPEDEF_FAR  *LPINT8;
typedef signed TYPEDEF_16BITS  INT16;
typedef signed TYPEDEF_16BITS  TYPEDEF_NEAR *PINT16;
typedef signed TYPEDEF_16BITS  TYPEDEF_FAR  *LPINT16;
#ifndef _BASETSD_H_ /*Guard against type defines in VC 6.0 */
typedef signed TYPEDEF_32BITS  INT32;
typedef signed TYPEDEF_32BITS  TYPEDEF_NEAR *PINT32;
#endif
typedef signed TYPEDEF_32BITS  TYPEDEF_FAR  *LPINT32;

/* typedef unsigned TYPEDEF_NATURAL  *PUINT; Defined by platform */
typedef unsigned TYPEDEF_8BITS    UINT8;
typedef unsigned TYPEDEF_8BITS    *PUINT8;
typedef unsigned TYPEDEF_8BITS    TYPEDEF_FAR * LPUINT8;
typedef unsigned TYPEDEF_16BITS   UINT16;
typedef unsigned TYPEDEF_16BITS   *PUINT16;
typedef unsigned TYPEDEF_16BITS    TYPEDEF_FAR * LPUINT16;


#ifndef _BASETSD_H_ /*Guard against type defines in VC 6.0 */
typedef unsigned TYPEDEF_32BITS   UINT32;
typedef unsigned TYPEDEF_32BITS   *PUINT32;
#endif
typedef unsigned TYPEDEF_32BITS    TYPEDEF_FAR * LPUINT32;

#ifdef TYPEDEF_64BITS
typedef TYPEDEF_64BITS   INT64;
typedef TYPEDEF_64BITS   *PINT64;
typedef unsigned TYPEDEF_64BITS    UINT64;
typedef unsigned TYPEDEF_64BITS    *PUINT64;
#endif

/* typedef TYPEDEF_8BITS CHAR; Defined by platform */

/* VP character for basic VP strings */
typedef CHAR TYPEDEF_STRINGCHAR;

typedef INT (WINAPI *PDLLMAIN)( HINSTANCE, DWORD, LPVOID);

#include <platcomm.h>

#endif /* _WIN32_PLAT_H_ */
