/*************************************************************************
*
*  platcomm.h
*
*  Build platform specfic types based on macros specified for
*  a platform.
*
*  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/platcomm.h#1 $
*
*  Copyright 1998-2015 Citrix Systems, Inc. 
*
************************************************************************/

#ifndef _PLATCOMM_H_
#define _PLATCOMM_H_

#pragma once

/*
 *  Define types that are nomally left to windows to define.
 */
#ifndef WINAPI
typedef unsigned TYPEDEF_NATURAL  UINT;
typedef signed TYPEDEF_NATURAL TYPEDEF_NEAR *PINT;
typedef signed TYPEDEF_NATURAL TYPEDEF_FAR  *LPINT;
typedef void   TYPEDEF_FAR  * LPVOID;
typedef TYPEDEF_NATURAL BOOL;
#endif

/*
 *  Legacy like types that may or may not be defined by other platforms
 *  like windows
 */

#ifndef LEGACY_TYPES_DEFINED
typedef unsigned TYPEDEF_8BITS UCHAR;
typedef unsigned TYPEDEF_16BITS USHORT;
typedef unsigned TYPEDEF_32BITS ULONG;

/*
 *  The old client headers used the far keyword, though in theory these
 *  would be near pointers.
 */
typedef void   TYPEDEF_FAR  * PVOID;
typedef CHAR   TYPEDEF_FAR  * PCHAR;
typedef UCHAR  TYPEDEF_FAR  * PUCHAR;
typedef USHORT TYPEDEF_FAR  * PUSHORT;
typedef ULONG  TYPEDEF_FAR  * PULONG;

typedef UINT   TYPEDEF_FAR  * LPUINT;
typedef USHORT TYPEDEF_FAR  * LPUSHORT;

typedef PCHAR  TYPEDEF_FAR * PPCHAR;
typedef PUCHAR TYPEDEF_FAR * PPUCHAR;

typedef UCHAR           BOOLEAN;
typedef BOOLEAN TYPEDEF_FAR   * PBOOLEAN;
typedef BOOLEAN TYPEDEF_FAR   * LPBOOLEAN;
typedef BOOL    TYPEDEF_FAR   * LPBOOL;
typedef BOOL *PBOOL;

#endif /* LEGACY_TYPES_DEFINED */

typedef PVOID  TYPEDEF_FAR  * PPVOID;
typedef ULONG  TYPEDEF_FAR  * LPULONG;
typedef CHAR   TYPEDEF_FAR  * LPCHAR;

/* VP string and VP character */
typedef TYPEDEF_STRINGCHAR *VPSTRING;
typedef TYPEDEF_STRINGCHAR VPCHAR;

/* VPSIZE defined  */

typedef struct _VPSIZE
{
    UINT32 uiWidth;
    UINT32 uiHeight;
    
} VPSIZE, TYPEDEF_FAR * PVPSIZE ;

/* VPPOINT defined  */
typedef struct _VPPOINT
{
    INT32  iX;
    INT32  iY;
    
} VPPOINT, TYPEDEF_FAR * PVPPOINT ;

/* VPRECT defined  */
typedef struct _VPRECT
{
    INT32  iX;
    INT32  iY;
    UINT32 uiWidth;
    UINT32 uiHeight;
} VPRECT, TYPEDEF_FAR * PVPRECT ;


/*
 *  General Virtual Platform Types
 */
typedef PVOID                 HND;
typedef HND TYPEDEF_FAR     * PHND;
typedef UINT32                VPSTATUS;

// Monitor layout structures
// duplicated in Client_Typedefs.h along with above VP structures
#define CLIENT_MAX_MONITORS 9
typedef struct _CLIENT_MONITOR
{
    VPRECT Rect;
    VPRECT ScaledRect;
    VPRECT WorkArea;
	VPRECT ScaledWorkArea;
    USHORT DevicePpi;
    USHORT ScaleFactor;
	USHORT OSAppliedScaleFactor;
} CLIENT_MONITOR;

typedef struct _CLIENT_MONITOR_LAYOUT
{
    UINT NumMonitors;
    UINT PrimaryMonitor;
    BOOL ScaledLocally;
	BOOL DoSpecialCreateCursorScaling;
	BOOL DoSpecialCreateIconIndirectScaling;
    CLIENT_MONITOR Monitors[CLIENT_MAX_MONITORS];
} CLIENT_MONITOR_LAYOUT, *PCLIENT_MONITOR_LAYOUT;

/*
 *  Function types
 */
typedef UINT32 (PWFCAPI PLIBPROCEDURE)();
typedef PLIBPROCEDURE TYPEDEF_FAR *PPLIBPROCEDURE;
typedef void (PWFCAPI PFNDELIVER)( PVOID, PVOID );

/*
 *  Miscellaneous Macros
 */

#ifndef NULL
#ifdef __cplusplus
#define NULL   0
#else
#define NULL   ((void *)0)
#endif
#endif

typedef UCHAR  TYPEDEF_FAR  * LPUCHAR;

#define MSB(x)     ((UINT8) (((x) >> 8) & 0x00ff))
#define LSB(x)     ((UINT8) ((x) & 0x00ff))

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) MIN(a,b)
#endif
#ifndef max
#define max(a,b) MAX(a,b)
#endif

/* Normal conversion format for
 * unsigned 64-bit integers in printf() strings.
 */

#ifndef UINT64_FORMAT
#define UINT64_FORMAT "%llu"
#endif

/*
 * STATIC is used for embeded drivers.  STATIC should not be defined
 * by default.  Embeded drivers define STATIC as static to keep their
 * function names within the drivers own scope.
 */
#ifndef STATIC
#define STATIC
#endif

/*
 *  Define CONST if it is not defined.
 */
#ifndef CONST
#define CONST  TYPEDEF_CONST
#endif

/*
 *  Sometimes we want to implement an interface, but we end up not using
 *  one or more of the parameters.  This macro will get rid of the compiler
 *  warnings with no ill effects.
 */
#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P)   (P)
#endif

#endif /* _PLATCOMM_H_ */
