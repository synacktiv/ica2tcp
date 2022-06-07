/*************************************************************************
 *
 *  Unix version of windos\platcall.h
 *
 *  Windows/DOS specific calling conventions
 *
 *  $Id$
 *
 *  Copyright 1999-2004 Citrix Systems, Inc. All Rights Reserved.
 *
 ************************************************************************/

#ifndef _UNIX_PLATCALL_H_
#define _UNIX_PLATCALL_H_
 
/*
 * Old calling convention macros.  These are deprecated as they are not
 * portable between WinDOS and Mac source. 
 */ 


#define _CAPI      TYPEDEF_CDECL
#define _PASCALAPI TYPEDEF_PASCAL
#define _FASTAPI   TYPEDEF_FASTCALL
#define _FAR_CAPI      TYPEDEF_CDECL TYPEDEF_FAR
#define _FAR_PASCALAPI TYPEDEF_PASCAL TYPEDEF_FAR
#define _FAR_FASTAPI   TYPEDEF_FASTCALL TYPEDEF_FAR
#define _INLINEAPI TYPEDEF_INLINE

#define _VPAPI  TYPEDEF_CDECL TYPEDEF_FAR TYPEDEF_LOADDS
#define _PVPAPI TYPEDEF_CDECL TYPEDEF_FAR *

#ifndef WFCAPI
#define WFCAPI TYPEDEF_CDECL TYPEDEF_FAR  TYPEDEF_LOADDS
#endif
#ifndef PWFCAPI
#define PWFCAPI TYPEDEF_CDECL TYPEDEF_FAR  *
#endif

#define WFCAPI_NOLOADDS TYPEDEF_CDECL TYPEDEF_FAR
#define PWFCAPI_NOLOADDS TYPEDEF_CDECL TYPEDEF_FAR *
#define WFCCALLBACK CALLBACK TYPEDEF_LOADDS


/*
 * New calling convention macros.  The parameterisation of the return type
 * allows its position relative to the calling convention keywords to be
 * controlled, which is important as they follow the type on WinDOS platforms
 * but precede it on the Mac.
 */
 
#define _CCALL(type)            type
#define _PASCALCALL(type)       TYPEDEF_PASCAL type
#define _FASTCALL(type)         type
#define _FAR_CCALL(type)        type
#define _FAR_PASCALCALL(type)   type
#define _FAR_FASTCALL(type)     type
#define WFCCALL(type)           type
#define PWFCCALL(type)          type
#define WFCCALL_NOLOADDS(type)  type
#define PWFCCALL_NOLOADDS(type) type
#define WFCCALLBACKCALL(type)   type

/*
 * Define _stdcall to nothing as it is not required for Unix
 */
#define _stdcall


#endif /* _UNIX_PLATCALL_H_ */
