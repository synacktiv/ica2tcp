/***************************************************************************
*
*   citrix.h
*
*   This file provides the basic macros and types common to all platforms,
*   and those defined for the particular platform that is being built.
*
*   The actual platform-specific definitions are included from the platform
*   sub-directory whose location is specified by the appropriate compiler
*   option.
*
*   $Id$
*
*   Copyright 1998-2005, 2009 Citrix Systems, Inc.  All Rights Reserved.
*
****************************************************************************/

#ifndef _CITRIX_H_
#define _CITRIX_H_

/*
 * Include platform-specific header files related to the platform
 * tools and the platform types.
 */
#include <plat.h>
#include <plattool.h>
#include <platsys.h>

/* Ensure we have an unsigned int type that is at least as wide as pointers. */

#ifndef UINTPTR_defined
typedef unsigned long int UINTPTR;
#define UINTPTR_defined
#endif

/*
 * Include common header files.
 */

#include <utilmacros.h>

#ifdef DEBUGALLOCATIONS
#include <dbgalloc.h>
#endif /* DEBUGALLOCATIONS */

#endif /* _CITRIX_H_ */
