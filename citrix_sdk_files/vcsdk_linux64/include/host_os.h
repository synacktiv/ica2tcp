/*[
 *****************************************************************************
 *
 *	Name:		host_os.h
 *
 *	$Id$
 *
 *	Purpose:	Defines things required to interface to this host's
 *			operating system.  This may be macros that map system
 *			or library calls, base/host/Mac/Unix interface, or
 *			defines that are required in order to keep system
 *			header files doing the right thing.
 *
 *	Copyright 1998-2015 Citrix Systems, Inc.  All rights reserved.
 *
 *****************************************************************************
]*/

#ifndef _HOST_OS_H_
#define _HOST_OS_H_

#define LINUX_OS

#define MallocH		<malloc.h>
#define StringH		<string.h>
#define StatH		<sys/stat.h>
#define TimeH		<time.h>
#define TypesH		<sys/types.h>
#define UnistdH		<unistd.h>
#define FcntlH		<fcntl.h>
#define VfsH        <sys/vfs.h>

/* Unix does not support this notion */
#define CALL_TYPE

#define host_malloc(size)	malloc(size)
#define host_calloc(num,size)	calloc(num,size)
#define host_realloc(ptr,size)	realloc(ptr,size)
#define host_free(ptr)		free(ptr)
#define host_memset     memset
#define host_memmove    memmove
#define host_memcpy    	memcpy
#define host_stat    	stat
#define host_bcopy(src,dst,len)	bcopy(src,dst,len)

#define host_getenv(str)	getenv(str)
#define host_setenv		setenv

#define host_access(path,mode)	access(path,mode)

#define host_open(name,flags,perms)  open(name,flags,perms)

#define host_AnsiUpper(str)     AnsiUpper(str)

#define DIR_DELIM "/"

#define host_socket_errno	errno
#define host_socket_close	close

/*
 * Setup host charateristics. /dev/random is always assumed to 
 * be present, so the entropy store isn't required.
 */
#define LITTLEND
#define OS_RANDOM_DEV           "/dev/random"

/* compiler pads structures to put shorts on even offsets */
#define USE_WIRE_PACKED_WRITE

/*
 * Setup host function names.
 */

#define stricmp(x,y) strcasecmp(x,y)
#define strnicmp(x,y,z) strncasecmp(x,y,z)
#define _strdup(x) strdup(x)

/* HP does have this, but I think it's a more recent addition to libc */
#define _vsnprintf(buf,size,fmt,arg)    vsprintf(buf,fmt,arg)

/* for debugging - allows editres(1) to see ncs resources */
#define host_XEditResCheckMessages _XEditResCheckMessages

/* This macro is used to avoid 'unused parameter' warnings.
 * On the Linux gcc gives warnings unless it is empty.
 */
#define UNREFERENCED_PARAMETER(p)

/* serial port hardware flow control */
#define CCMHOST_CRTSCTS CRTSCTS

/* shared memory behaviour with IPC_RMID */
#define SHM_DELETE_ON_LAST_REF

/* 
 * types uint8_t, etc. already defined.
 */

#define __INTTYPES_INCLUDED

#define EXTRA_TCP_OPTIONS 12

#endif /* _HOST_OS_H_ */
