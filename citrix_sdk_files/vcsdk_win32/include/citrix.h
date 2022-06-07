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
*   $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/citrix.h#1 $
*
*   Copyright 1998-2015 Citrix Systems, Inc.  
*
****************************************************************************/

#ifndef _CITRIX_H_
#define _CITRIX_H_

#pragma once

/*
 * Include platform-specific header files related to the platform
 * tools and the platform types.
 */
#include <plat.h>
#include <plattool.h>
#include <platsys.h>

/*
 * Include common header files.
 */

#include <utilmacros.h>

#ifdef DEBUGALLOCATIONS
#include <dbgalloc.h>
#endif /* DEBUGALLOCATIONS */

#endif /* _CITRIX_H_ */
