/***************************************************************************
*
*  Plattool.h
*
*  Header file that can define types common to the windows and dos
*  platforms.  This file should contain items that are most likely
*  common to the tools associated with the build machine.
*
* Copyright 1998-2015 Citrix Systems, Inc. 
***************************************************************************/

#ifndef _WINDOS_PLATTOOL_H_
#define _WINDOS_PLATTOOL_H_

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

/*
 *  Pragma Macros
 */
#ifdef MAKEDBG
#define MKDBG(x) message("MKDBG: " x)
#else
#define MKDBG(x)
#endif

#ifdef MAKEVERB
#define MKVERB(x) message("MKDBG: " x)
#else
#define MKVERB(x)
#endif

/*
 *  Pragmas for the WinDos platform
 */

/* Disable inline assembly warning */
#ifdef PLATFORM_PRAGMAS
#pragma warning(disable:4704)
#endif


#endif /* _WINDOS_PLATTOOL_H_ */
