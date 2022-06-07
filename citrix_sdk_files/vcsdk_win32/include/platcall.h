/*************************************************************************
 *
 *  windos\platcall.h
 *
 *  Windows/DOS specific calling conventions
 *
 *  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/windos/platcall.h#1 $
 *
 * Copyright 1999-2015 Citrix Systems, Inc. 
 *
 ************************************************************************/

#ifndef _WINDOS_PLATCALL_H_
#define _WINDOS_PLATCALL_H_

/*
 * Old calling convention macros.  These are deprecated as they are not
 * portable between WinDOS and Mac source. 
 */
#define _CAPI      TYPEDEF_CDECL
#define _PASCALAPI TYPEDEF_PASCAL
#define _FASTAPI   TYPEDEF_FASTCALL
#define _FAR_CAPI      TYPEDEF_CDECL TYPEDEF_FAR TYPEDEF_LOADDS
#define _FAR_PASCALAPI TYPEDEF_PASCAL TYPEDEF_FAR
#define _FAR_FASTAPI   TYPEDEF_FASTCALL TYPEDEF_FAR
#define _INLINEAPI TYPEDEF_INLINE

#ifndef _VPAPI
#define _VPAPI  TYPEDEF_CDECL TYPEDEF_FAR TYPEDEF_LOADDS
#endif
#ifndef _PVPAPI
#define _PVPAPI TYPEDEF_CDECL TYPEDEF_FAR *
#endif
#ifndef WFCAPI
#define WFCAPI TYPEDEF_CDECL TYPEDEF_FAR TYPEDEF_LOADDS
#endif
#ifndef PWFCAPI
#define PWFCAPI TYPEDEF_CDECL TYPEDEF_FAR *
#endif
#define WFCAPI_NOLOADDS TYPEDEF_CDECL TYPEDEF_FAR
#define PWFCAPI_NOLOADDS TYPEDEF_CDECL TYPEDEF_FAR *
#define WFCCALLBACK CALLBACK TYPEDEF_LOADDS
#ifndef STDCALL
#define STDCALL __stdcall
#endif
/*
 * New calling convention macros.  The parameterisation of the return type
 * allows its position relative to the calling convention keywords to be
 * controlled, which is important as they follow the type on WinDOS platforms
 * but precede it on the Mac.
 */
#define _CCALL(type)            type TYPEDEF_CDECL
#define _PASCALCALL(type)       type TYPEDEF_PASCAL
#define _FASTCALL(type)         type TYPEDEF_FASTCALL
#define _FAR_CCALL(type)        type TYPEDEF_CDECL TYPEDEF_FAR
#define _FAR_PASCALCALL(type)   type TYPEDEF_PASCAL TYPEDEF_FAR
#define _FAR_FASTCALL(type)     type TYPEDEF_FASTCALL TYPEDEF_FAR
#define WFCCALL(type)           type TYPEDEF_CDECL TYPEDEF_FAR TYPEDEF_LOADDS
#define PWFCCALL(type)          type TYPEDEF_CDECL TYPEDEF_FAR *
#define WFCCALL_NOLOADDS(type)  type TYPEDEF_CDECL TYPEDEF_FAR
#define PWFCCALL_NOLOADDS(type) type TYPEDEF_CDECL TYPEDEF_FAR *
#define WFCCALLBACKCALL(type)   type CALLBACK TYPEDEF_LOADDS

#endif /* _WINDOS_PLATCALL_H_ */
