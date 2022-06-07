/*************************************************************************
*
* win32\platclib.h
*
* WIN32 specific Header file for C runtime library routines 
*
* Copyright 2015 Citrix Systems, Inc. 
*
*************************************************************************/
#ifndef _PLATCLIB_H_
#define _PLATCLIB_H_

typedef struct _iobuf FILE;

#include <clibcomm.h>

/*
 *  Force strcat, strcmp, strcpy, and strlen to be function 
 *  that will be called instead of inline code
 */
#pragma function( strcat, strcmp, strcpy, strlen )

/*
 *  Define Macros to call standard c-library functions
 */

#define memicmp         _memicmp
#define stricmp         _stricmp
#define strdup          _strdup
#define strnicmp        _strnicmp
#define open            _open
#define lseek           _lseek
//#define write           _write
#define read            _read
#define close           _close
#define access          _access
#define mkdir           _mkdir
#define rmdir           _rmdir
#define GetLastErrno()  errno

#endif /* _PLATCLIB_H_ */
