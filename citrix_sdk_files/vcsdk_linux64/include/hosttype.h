/*[
 ******************************************************************************
 *
 *	Name:		host_types.h
 *
 *	$Id$
 *	Purpose:	Defines basic types.  These depend on the
 *			processor and compiler.  Other types should be
 *			defined in terms of these types (plus void).
 *
 #	Copyright 1998-2014 Citrix Systems, Inc.  All rights reserved.
 *
 ******************************************************************************
]*/

#ifndef _HOST_TYPES_H_
#define _HOST_TYPES_H_

#include <stdint.h>

/* 8 bit types */
#if !defined(_UNIX_PLAT_H_) /* avoid clash with platcomm.h */
typedef char		CHAR;
#endif

typedef signed char	IS8;
typedef unsigned char	IU8;
typedef signed long	ISM8;
typedef unsigned long	IUM8;

/* 16 bit types */
typedef signed short	IS16;
typedef unsigned short	IU16;
typedef signed long	ISM16;
typedef unsigned long	IUM16;

/* 32 bit types */
typedef signed int	IS32;
typedef unsigned int	IU32;
typedef signed int	ISM32;
typedef unsigned int	IUM32;
typedef uintptr_t       UINTPTR;
#define UINTPTR_defined

/* Boolean  - use 32 bit type for best performance */
typedef IU32		IBOOL;

typedef long long       IS64;
typedef IS64            ISM64;
typedef unsigned long long IU64;
typedef IU64            IUM64;

#endif /* _HOST_TYPES_H_ */
