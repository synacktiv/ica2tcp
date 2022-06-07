/*************************************************************************
*
*  unix\platsys.h
*
*  Version: $Id: $
*
*  Basic platform abstractions
*
*  Copyright (c) 1998 - 2000 Citrix Systems, Inc. All Rights Reserved.
*
*************************************************************************/

#ifndef _UNIX_PLATSYS_H_
#define _UNIX_PLATSYS_H_

/*
 * Platform specfic prototypes
 */

/*
 * Platform specific macros
 */

#define CtxCharUpper(p)	StrUpper(p)

#define _strnicmp(x,y,z) strnicmp(x,y,z)

/* #define BROWSER_CODEPAGE 932  (shiftJIS) */
#define BROWSER_CODEPAGE 1252

/* To allow us to build on machines that don't have Motif 2.0+ 
   we define this resource label.
*/

#ifndef XmNunselectColor
#define XmNunselectColor "unselectColor"
#endif

#endif /* _UNIX_PLATSYS_H_ */
