/*[
 ******************************************************************************
 *
 *	Name:		basetype.h
 *
 *	$Id$
 *
 *	Purpose:	Defines a number of data types in terms of the basic
 *			insignia types (and void).  These types are all old
 *			insignia types, or are required for code obtained from
 *			third parties (such as Citrix).  This file covers all
 *			hosts.
 *
 *	Copyright 1998-2004, 2006 Citrix Systems, Inc. All rights reserved.
 *
 ******************************************************************************
]*/

#ifndef _BASE_TYPES_H_
#define _BASE_TYPES_H_

#ifndef FALSE
#define FALSE	0
#endif /* FALSE */ 

#ifndef TRUE
#define TRUE	(!FALSE)
#endif /* TRUE */

/*
 * Insignia Standard Storage Classes
 */

#define GLOBAL				/* Defined as nothing */
#define LOCAL	static			/* Local to the source file */
#define SAVED	static			/* For local static variables */
#define OPTIONAL
#define VOID void
/*
 * These seem to be used by Microsoft (Licensing code) in parameters
 */
#define IN
#define OUT
#define INOUT

/*
 * A nice word to replace extern
 */

#define IMPORT	extern

/*
 *  Mappings of older insignia types and types inherited form Citrix
 */

typedef void * 		LPVOID;

#ifndef NULL
#define NULL ((void *) 0)
#endif /* NULL */


typedef char *		LPSTR;
typedef char *		LPCTSTR;
#if 0
typedef wchar_t		WCHAR;
#else
typedef unsigned short	WCHAR;
extern int wchar_check_size[(2 == sizeof(WCHAR)) ? 1 : -1];
#endif
typedef WCHAR 		*PWCHAR, *LPWSTR;

#ifndef X_BOOLBYTE_TYPES
typedef IU8		BYTE;
typedef BYTE		*LPBYTE;
#endif /* X_BOOLBYTE_TYPES */

#define BYTEWIDTH 8
typedef IU8		UBYTE;

#define UNALIGNED

#if !defined(_UNIX_PLAT_H_) && !defined(_MAC_PLAT_H_)
/* avoid clashes with platcomm.h */
typedef IS32		INT;
typedef	unsigned char	UINT8 ;
typedef IU8		UCHAR;
typedef IS16		SHORT;
typedef IU16		USHORT;
typedef USHORT *	PUSHORT;
typedef IU32		ULONG;
typedef ULONG *		PULONG;
typedef ULONG *		LPULONG;   /* the PC unsigned long far ptr */
typedef void *		PVOID;
typedef char *		PCHAR;
typedef UCHAR *		PUCHAR;
typedef IU32 *		LPUINT;

/* The following depends on the above */
typedef UCHAR *		PBYTE;
#else  /* avoid clashes with platcomm.h */
/*typedef unsigned short	UINT16;*/
/*typedef unsigned int	UINT32;*/
#endif /* avoid clashes with platcomm.h */

typedef IS32 *		LPINT;
typedef IU32		UINT;

#if defined (_UNIX_PLAT_H_)
typedef IS32        LONG;
#endif

typedef IS32		LRESULT;
typedef IS32		LPARAM;


typedef void *		HANDLE;
typedef HANDLE *	LPHANDLE;
typedef HANDLE *	PHANDLE;
typedef HANDLE		HGLOBAL;
typedef void *		HINSTANCE;
#define HBITMAP		HANDLE

#ifndef X_BOOLBYTE_TYPES
typedef unsigned char	BOOL;   /* Compatible with X Windows. */
#endif /* X_BOOLBYTE_TYPES */

/*
 * Eliminate PC things from code.
 */
#undef NEAR
#undef FAR
#undef far
#undef cdecl
#undef _cdecl
#undef __cdecl
#undef CDECL
#undef _CDECL

#define NEAR 
#define FAR 
#define far
#define cdecl
#define _cdecl
#define __cdecl
#define CDECL
#define _CDECL

#if defined(macintosh) || defined(MacOrUnix)
#undef WIN32
#else
#define WIN32
#endif

typedef void * HWND;

/* typedefs for manipluating pointers, e.g. for alignment testing */
typedef    void *          IHP;
typedef unsigned long      IHPE;

/* Allow X type pixel values on Unix machines. */
 
#if defined(unix)
#define PIXEL_TYPE unsigned long
#endif

/* define the signature words to allow the OEMpoke utility to locate the OEM ID. 
	This is the word sandwiched between these */
#define OEMSIG1 0xAAAA
#define OEMSIG2 0x5555

/*	Macro to allow non-aligned assignments 
 *	This is needed because variables of two bytes or more are copied
 *	into a buffer at arbitary locations. This causes alignment errors
 *	on some processors, usually resulting in bus errors.
 *
 *  NB. Care must still be taken in accessing multibyte members of 
 *	'structures' which are really part of a buffer.
 *
 *	Note that source and destination for SAFEASSIGN must be of correct type 
 *	as bytes will be copied from and to the address of the data item
 *	regardless of how long it is.
 */
#if 0
/* moved to cmacro.h */
#define SAFEASSIGN(dst, src, type) {	ASSERT(sizeof(src) == sizeof(type), sizeof(src)); \
					ASSERT(sizeof(dst) == sizeof(type), sizeof(dst)); \
			host_bcopy((char *) &(src), (char *) &(dst), sizeof(type)); }
#endif

// This is also defined in CL Mgr so it is a re-define when including CL Mgr headers
#ifndef UNUSED
#define UNUSED(x) /* UNUSED MACRO */
#endif

#define CP_ACP  0

#endif /* _BASE_TYPES_H_ */
