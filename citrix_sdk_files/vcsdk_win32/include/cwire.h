/***************************************************************************
*
*  cwire.h
*
*  This module contains swapping macros for bigendian platforms
*
*   Written by: davidh@ctxuk.citrix.com
*   Date: 15 June 1999
*
*   revision: $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/cwire.h#1 $
*
*   Copyright 1999-2015 Citrix Systems, Inc. 
*
****************************************************************************/

#ifndef _CWIRE_H_
#define _CWIRE_H_

#pragma once

/* Wire macro definitions.											*/
/*																	*/
/* For write operations the destination pointer is incremented. 	*/
/* For read operations the source pointer is incremented. 			*/
/* Its is intended that the pointer to the wire buffer that is 		*/
/* incremented.														*/




#define ctx_wire_write8(src, dst)	{_ctx_copy8(((LPBYTE) src), ((LPBYTE) dst)) dst += 1 ; }
#define ctx_wire_write16(src, dst)	{_ctx_swap16(((LPBYTE) src), ((LPBYTE) dst)) dst += 2 ; }
#define ctx_wire_write32(src, dst)	{_ctx_swap32(((LPBYTE) src), ((LPBYTE) dst)) dst += 4 ; }

#define ctx_wire_read8(src, dst)	{_ctx_copy8(((LPBYTE) src), ((LPBYTE) dst)) src += 1 ; }
#define ctx_wire_read16(src, dst)	{_ctx_swap16(((LPBYTE) src), ((LPBYTE) dst)) src += 2 ; }
#define ctx_wire_read32(src, dst)	{_ctx_swap32(((LPBYTE) src), ((LPBYTE) dst)) src += 4 ; }
#define ctx_wire_read64(src, dst)	{_ctx_swap64(((LPBYTE) src), ((LPBYTE) dst)) src += 8 ; }






#define _ctx_copy8(src, dst) \
	if (src != dst) \
		dst [0] = src [0] ; 



#define _ctx_swap16(src, dst) \
	if (src == dst) { \
		BYTE temp = src[0]; \
		src[0] = src[1]; \
		src[1] = temp; \
	} else { \
		dst[0] = src[1]; \
		dst[1] = src[0]; \
	} 

	

#define _ctx_swap32(src, dst) \
	if (src == dst) { \
		BYTE temp = src[0]; \
		src[0] = src[3]; \
		src[3] = temp; \
		temp = src[1]; \
		src[1] = src[2]; \
		src[2] = temp; \
	} else { \
		dst[0] = src[3]; \
		dst[3] = src[0]; \
		dst[1] = src[2]; \
		dst[2] = src[1]; \
	} 

#define _ctx_swap64(src, dst) \
	if (src == dst) { \
		BYTE temp = src[0]; \
		src[0] = src[7]; \
		src[7] = temp; \
		temp = src[1]; \
		src[1] = src[6]; \
		src[6] = temp; \
		temp = src[2]; \
		src[2] = src[5]; \
		src[5] = temp; \
		temp = src[3]; \
		src[3] = src[4]; \
		src[4] = temp; \
	} else { \
		dst[0] = src[7]; \
		dst[1] = src[6]; \
		dst[2] = src[5]; \
		dst[3] = src[4]; \
		dst[4] = src[3]; \
		dst[5] = src[2]; \
		dst[6] = src[1]; \
		dst[7] = src[0]; \
	} 





#endif /* _CWIRE_H_ */

