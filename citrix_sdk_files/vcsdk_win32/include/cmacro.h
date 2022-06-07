/***************************************************************************
*
*  cmacro.h
*
*  This module contains wire marshalling function call macros
*
*   revision: $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/cmacro.h#1 $
*
*   Copyright 1999-2015 Citrix Systems, Inc.  
*
****************************************************************************/

#ifndef _CMACRO_H_
#define _CMACRO_H_

#pragma once



#ifdef BIGEND /* this will be defined for bigendian client builds */



/* Macro for allocating temporary stack based variable to swap */
/* data into and a pointer to return the data     */

#define WIRE_PTR(type, localptr) type temp##type, *localptr


/* Macros for swapping data held in the wirebuffer into a local  */
/* variable when reading from the wire and swapping data inplace */
/* when writing to the wire                                      */

#define WIRE_READ(type, localptr, wirebuffer) localptr = &temp##type; Marshall_Read_##type (wirebuffer, localptr)
#define WIRE_WRITE(type, wirebuffer, buffersize) Marshall_Write_##type (wirebuffer, buffersize)
#define WIRE_PACKED_WRITE(type, wirebuffer, buffersize) \
    WIRE_WRITE(type, wirebuffer, buffersize)

/* Variant used when a structure contains a pointer to another structure
 * Memory for the local structure must be allocated as its size may differ from
 * the host's.
 */
#define WIRE_READ_STRUCTURES(type, localptr, wirebuffer, n) \
	if(Mem_alloc(MEM_GLOBAL_POOL,sizeof(type)*(n),(PVOID*) &localptr)) { \
	    TRACE((TC_FONT, TT_ERROR, " Couldn't allocate " #type " memory"));\
	    return; \
	} \
	Marshall_Read_##type(wirebuffer, localptr, n)

/* Alternative approach: swap a structure in-place after it has
 * been copied from the wire.  This is used in the TW VD where
 * everything is copied from the wire buffer to assemble it,
 * as TW commands can cross buffer boundaries.
 * THIS METHOD SHOULD BE USED IN THE THINWIRE MODULE ONLY !!!
 */

#define SWAP_IN_PLACE(type, ptr) Swap_##type(ptr);

#define Swap_WORD(word) word = ((word >> 8) & 0x00ff) | ((word << 8) & 0xff00);
#define Swap_USHORT Swap_WORD
#define Swap_ULONG Swap_DWORD

/* Yet another approach: take the bytes of a short, etc. from the wire and
 * swap them into a value which can be assigned on this platform.
 * The macros WIRE_VALUEx can be used safely where the parameter's
 * alignment is unknown.
 * THIS METHOD SHOULD BE MADE OBSOLETE BY USING THE FINAL SWAPPING METHOD
 * BELOW : wire_read8, wire_read16 and wire_read32 !!!
 */
#define WIRE_VALUE8(b)  (b)

#define WIRE_VALUE16(w) ((((PUCHAR) &w)[1]<<8) | ((PUCHAR) &w)[0])

#define WIRE_VALUE32(l) ((((PUCHAR) &l)[3]<<24) | (((PUCHAR) &l)[2]<<16) | \
                         (((PUCHAR) &l)[1]<<8)  |  ((PUCHAR) &l)[0]       )

#else /* !BIGEND */

#if NEED_ALIGNMENT || defined(ALIGNMENT_REQUIRED)

/* Need to be a bit careful if the data needs to be word- or dword-aligned.
 */

#define WIRE_PTR(type, localptr) type temp##type, *localptr
#define WIRE_READ(type, localptr, wirebuffer) localptr = &temp##type;memcpy(localptr,wirebuffer,sizeof(type))
#else

/* Macro for allocating temporary stack based pointer to return  */
/* the data. Just pass through.                                  */

#define WIRE_PTR(type, localptr) type *localptr

/* Stubs for the above macros on platforms that don't need byte
 * swapping or alignment. Only the WIRE_PACKED_WRITE macro (potentially copies data).
 */

#define WIRE_READ(type, localptr, wirebuffer) localptr = (type *)((void*)wirebuffer)
#endif /* NEED_ALIGNMENT || defined(ALIGNMENT_REQUIRED) */

#define WIRE_WRITE(type, wirebuffer, buffersize) buffersize


/* Some little-endian platforms, eg Linux, need to pack structures as they are
 * copied to the wire. Others, eg Win32, don't.
 * Allow this behaviour to be selected in platform-specific header files
 * by defining USE_WIRE_PACKED_WRITE for the Linux, etc.
 */
#ifdef USE_WIRE_PACKED_WRITE

#define WIRE_PACKED_WRITE(type, wirebuffer, buffersize) \
    Marshall_Write_##type (wirebuffer, buffersize)

#else

#define WIRE_PACKED_WRITE(type, wirebuffer, buffersize) buffersize

#endif /* USE_WIRE_PACKED_WRITE */

#define WIRE_READ_STRUCTURES(type, localptr, wirebuffer, n) \
  localptr = (type *)(wirebuffer)

#define SWAP_IN_PLACE(type, ptr)

#define WIRE_VALUE8(b)  (b)
#if NEED_ALIGNMENT || defined(ALIGNMENT_REQUIRED)
#define WIRE_VALUE16(w) ((((PUCHAR) &w)[1]<<8) | ((PUCHAR) &w)[0])
#define WIRE_VALUE32(l) ((((PUCHAR) &l)[3]<<24) | (((PUCHAR) &l)[2]<<16) | \
                         (((PUCHAR) &l)[1]<<8)  |  ((PUCHAR) &l)[0]       )
#else
#define WIRE_VALUE16(w) (w)
#define WIRE_VALUE32(l) (l)
#endif
#endif /* !BIGEND */

/* Perform an alignment-safe multi-byte assignment by copying the
 * individual bytes. 
 * (The previous use of Mem_cpy() here ran into problems with over-optimistic
 * optimisation on the Sparc compiler which substituted a load-store.)
 */
#define SAFEASSIGN(dst, src, type) {                                    \
    size_t size;                                                        \
    PUCHAR  pSrc, pDst;                                                 \
    ASSERT(sizeof(src) == sizeof(type), sizeof(src));                   \
    ASSERT(sizeof(dst) == sizeof(type), sizeof(dst));                   \
    for (pSrc = (PUCHAR)&src, pDst = (PUCHAR)&dst, size = sizeof(type); \
            size--;)                                                    \
        *pDst++ = *pSrc++;                                              }

/*
 * The following methods to read and write data from and to
 * the wire are favourable to all previous ones for the following
 * reasons :
 * - It does not distinguish between alignment problems, packing
 *   problems and swapping needs. Rather, it combines all three
 *   concerns into one solution.
 * - It is faster than the swapping method in cwire.h
 * - It allows to assign a 2 byte wire value to a 4 byte variable
 * - It is platform independent, i.e. the method could also be used
 *   on little-endian machines without resulting in wrong data.
 */

/*
 * Read data from the wire :
 * Example : Assume a structure
 * struct example {
 *     unsigned char      byte;
 *     unsigned int       dword;
 *     unsigned short int word;
 * } mystruct;
 * Then fill it in with
 * mystruct.byte  = wire_read8 (pData);
 * mystruct.dword = wire_read32(pData);
 * mystruct.word  = wire_read16(pData);
 */

#define DIRECT_WIRE_READ8(pData) *pData++

#if defined(BIGEND) || defined (ALIGNMENT_REQUIRED)

#define DIRECT_WIRE_READ16(pData) pData[0] | pData[1] << 8; pData += 2
#define DIRECT_WIRE_READ32(pData) pData[0] | pData[1] << 8 | \
                           pData[2] << 16 | pData[3] << 24; pData += 4

#else

#define DIRECT_WIRE_READ16(pData) *(UINT16 *)pData; pData += 2
#define DIRECT_WIRE_READ32(pData) *(UINT32 *)pData; pData += 4

#endif

/*
 * Write data to the wire :
 * Example : Assume a structure
 * struct example {
 *     unsigned char      byte;
 *     unsigned int       dword;
 *     unsigned short int word;
 * } mystruct;
 * Then write its contents to the wire with
 * DIRECT_WIRE_WRITE8 (pData, mystruct.byte);
 * DIRECT_WIRE_WRITE32(pData, mystruct.dword);
 * DIRECT_WIRE_WRITE16(pData, mystruct.word);
 */

#define DIRECT_WIRE_WRITE8(pData, src8) *pData++ = src8


#if defined(BIGEND) || defined (ALIGNMENT_REQUIRED)

#define DIRECT_WIRE_WRITE16(pData, src16) pData[0] = src16 & 0xff; \
                                          pData[1] = (src16 >> 8) & 0xff; \
                                          pData += 2
#define DIRECT_WIRE_WRITE32(pData, src32) pData[0] = (UCHAR)(src32 & 0xff); \
                                          pData[1] = (UCHAR)((src32 >> 8) & 0xff); \
                                          pData[2] = (UCHAR)((src32 >> 16) & 0xff); \
                                          pData[3] = (UCHAR)((src32 >> 24) & 0xff); \
                                          pData += 4

#else

#define DIRECT_WIRE_WRITE16(pData, src16) *(UINT16 *)pData = src16; pData += 2
#define DIRECT_WIRE_WRITE32(pData, src32) *(UINT32 *)pData = src32; pData += 4

#endif

#endif /* _CMACRO_H_ */











#ifdef ImplementationNotes

/*
 *  Notes for implementors:
 *
 *
 *  READING DATA FROM THE WIRE
 *  --------------------------
 *
 *  Reading data from the wire, by which we mean the ica buffers after the 
 *  reducer has unpacked it, and it has been passed to a high enough level 
 *  for us to correctlty identify the structure of the data. When reading 
 *  data from the wire it is always swapped into an auto matic variable. 
 *  This is to solve potenial packing and allignment problems with accessing 
 *  the wire buffer on odd boundries. Consider the following example of the 
 *  wire buffer containing a list.
 */

typedef struct _LIST_DATA 
{
    ULONG data ;
} 
LIST_DATA, *PLIST_DATA ;


typedef struct _LIST_HEADER
{
    ULONG byteCount ;  /* size of the data following the header */
} 
LIST_HEADER, *PLISTHEADER ;



void ReadList (PUCHAR* pWireBuffer)
{
	WIRE_PTR(LIST_HEADER, localListHeader) ;
	WIRE_READ(LIST_HEADER, localListHeader, pWireBuffer) ;

	pWireBuffer += sizeof (LIST_HEADER) ;

	while (localListHeader->byteCount)
	{
		WIRE_PTR(LIST_DATA, localListData) ;
		WIRE_READ(LIST_DATA, localListData, pWireBuffer) ;

		DoSomethingWithTheData (localListData->data) ;
		
		pWireBuffer += sizeof (LIST_DATA) ;
		localListHeader->byteCount -= sizeof (LIST_DATA) ;
	}
}



/* For BIGENDIAN platforms the above code will expand to:
 *
 *
 *  void ReadList (PUCHAR* pWireBuffer)
 *  {
 *  	LIST_HEADER tempLIST_HEADER, *localListHeader ;
 *  	
 *  	localListHeader = &tempLIST_HEADER ;
 *  	Marshall_Read_LIST_HEADER (pWireBuffer, localListHeader) ;
 *
 *  	pWireBuffer += sizeof (LIST_HEADER) ;
 *
 *  	while (localListHeader->byteCount)
 *  	{
 *	  LIST_DATA tempLIST_DATA, *localListData ;
 *
 *	  localListData = &tempLIST_DATA ;
 *	  Marshall_Read_LIST_DATA (pWireBuffer, localListData) ;
 *
 *	  DoSomethingWithTheData (localListData->data) ;
 *	  
 *	  pWireBuffer += sizeof (LIST_DATA) ;
 *	  localListHeader->byteCount -= sizeof (LIST_DATA) ;
 *  	}
 *  }
 */



/* For LITTLENDIAN platforms the above code will expand to:
 *
 *  void ReadList (PUCHAR* pWireBuffer)
 *  {
 *  	LIST_HEADER* localListHeader ;
 *  	
 *  	localListHeader = (LIST_HEADER*)((void*)pWireBuffer) ;
 *
 *  	pWireBuffer += sizeof (LIST_HEADER) ;
 *
 *  	while (localListHeader->byteCount)
 *  	{
 *	  LIST_DATA* localListData ;
 *	  
 *	  localListData = (LIST_DATA*)((void*)pWireBuffer) ;
 *
 *	  DoSomethingWithTheData (localListData->data) ;
 *	  
 *	  pWireBuffer += sizeof (LIST_DATA) ;
 *	  localListHeader->byteCount -= sizeof (LIST_DATA) ;
 *  	}
 *  }
 */



/* For the BIGENDIAN version two routines Marshall_Read_LIST_HEADER and 
 * Marshall_Read_LIST_DATA have to be provided. The name of the routines 
 * are defined by the macro. An example of the routine would be:
 */

void Marshall_Read_LIST_DATA(LPVOID input, LPVOID output) 
{
	LPBYTE readPtr = (LPBYTE)input ;
	PLIST_DATA writePtr = (PLIST_DATA)output ;
	
	ctx_wire_read32(readPtr, &writePtr->data) ;	
}

/* The macro ctx_wire_read32 is defined in "cwire.h" along with other macro's 
 * for 16 and 8 bit quantities. 

 * A potential problem that has to be considered with this sort of code is 
 * using the sizeof() operator to increment the pointers and byte counts.
 * All data coming from the server is packed on byte boundries with no padding 
 * added to the end of the structures. If the structures you are using are not 
 * exact muliples of 4 bytes and structure members aren't naturally aligned
 * then the sizeof() operator will give different results on different
 * platforms. It is much safer to declare a constant that is set to the size
 * of the structure and use that for incrementing.
 * For this example it's not necessary but would look like this:
 * 
 * 
 * #define packed_size_of_SOME_STRUCT 23
 * .
 * .
 * .
 * pWireBuffer += packed_size_of_SOME_STRUCT ;
 * 
 */



/* WRITING DATA TO THE WIRE
 * ------------------------
 *
 *
 * This example is using the same data structure as for the reading example. 
 * This example assumes the wire buffer already contains the unswapped data. 
 * Also assumed is that no packing of the data is required, that is the 
 * structures are multiples of 4 bytes and naturally aligned. See below for 
 * a bespoke example for structures that are not.
 */



USHORT WriteList (PUCHAR* pWireBuffer)
{
	ULONG localByteCount = ((PLIST_HEADER) pWireBuffer)->byteCount ;
	USHORT packedSize = WIRE_WRITE(LIST_HEADER, pWireBuffer, sizeof(LIST_HEADER)) ;

	pWireBuffer += sizeof (LIST_HEADER) ;

	while (localByteCount)
	{
		packedSize += WIRE_WRITE(LIST_DATA, pWireBuffer, sizeof(LIST_DATA)) ;

		pWireBuffer += sizeof (LIST_DATA) ;
		localByteCount -= sizeof (LIST_DATA) ;
	}
	
	return packedSize ;
}



/* For example this routine would be called in a similar situation to
 * that given below, error handling code removed for clarity:
 */



	/*
	 *  Marshal output buffer
	 */
	dataSize = WriteList (pBuffer) ;

	/*
	 *  Try and get the output buffer
	 */
	rc = OutBufReserve( Wd, dataSize+4 );

	/*
	 *  Append Virtual write header
	 */
	rc = AppendVdHeader( Wd, (USHORT)VirtualxChannel, dataSize );

	/*
	 *  Append data to output buffer
	 */
	 rc = OutBufAppend( Wd, pBuffer, dataSize) ;

	/*
	 *  Write  outbuf
	 */
	rc = OutBufWrite( Wd );





/* For BIGENDIAN platforms the WriteList(...) routine will expand to:
 *
 *
 *   USHORT WriteList (PUCHAR* pWireBuffer)
 *   {
 *   	ULONG localByteCount = ((PLIST_HEADER) pWireBuffer)->byteCount ;
 *  	USHORT packedSize = Marshall_Write_LIST_HEADER (pWireBuffer, sizeof(LIST_HEADER)) ;
 *
 *  	pWireBuffer += sizeof (LIST_HEADER) ;
 *
 *  	while (localByteCount)
 *  	{
 *  		packedSize += Marshall_Write_LIST_DATA (pWireBuffer, sizeof(LIST_DATA)) ;
 *
 *  		pWireBuffer += sizeof (LIST_DATA) ;
 *  		localByteCount -= sizeof (LIST_DATA) ;
 *  	}
 *  	
 *  	return packedSize ;
 *  }
 */




/* For LITTLENDIAN platforms the WriteList(...) routine will expand to:
 *
 *
 *	USHORT WriteList (PUCHAR* pWireBuffer)
 *  {
 *  	ULONG writeCount = 0 ;
 *  	ULONG localByteCount = ((PLIST_HEADER) pWireBuffer)->byteCount ;
 *  	
 *  	USHORT packedSize = sizeof(LIST_HEADER) ;
 *
 *  	pWireBuffer += sizeof (LIST_HEADER) ;
 *
 *  	while (writeCount < localByteCount)
 *  	{
 *  		packedSize += sizeof(LIST_DATA) ;
 *
 *  		pWireBuffer += sizeof (LIST_DATA) ;
 *  		writeCount += sizeof (LIST_DATA) ;
 *  	}
 *  	
 *  	return packedSize ;
 *  }
 */



/* For the BIGENDIAN version two routines Marshall_Write_LIST_HEADER and 
 * Marshall_Write_LIST_DATA have to be provided. The name of the routines 
 * are defined by the macro. An example of the routine would be:
 */

	USHORT Marshall_Write_LIST_DATA(PLIST_DATA input, USHORT inputSize) 
	{
		LPBYTE writePtr = (LPBYTE)input ;

		ctx_wire_write32(&input->data, writePtr) ;
		
		return ((USHORT)(writePtr - (LPBYTE)input)) ;
	}


/* The macro ctx_wire_write32 is defined in "cwire.h" along with other macros
 * for 16 and 8 bit quantities. 
 * 
 * 
 * Note the local copy of the byteCount member of the list head structure.
 * For the bigendian implementation you could not use the actual structure
 * member's value because it will have been trashed as it was swapped.
 * 
 * 
 * If the list structures were not naturally alligned, or multples of 4 bytes
 * and the platform the client is designed for doesn't support packing on
 * byte boundries then a more bespoke medthod would have to be used.
 * The sizeof() operator here returns the size of the structure based on
 * the client packing. This assumes that the data required by the server
 * is packed on byte boundaries.
 */

	#define packed_sizeof_LIST_DATA 4

	USHORT WriteList (PUCHAR* pWireBuffer)
	{
		LPBYTE writePtr = (LPBYTE)pWireBuffer ;
		PLIST_DATA readPtr = (LPBYTE)pWireBuffer ;
		
		/* take a local copy of the non packed size */
		ULONG localByteCount = ((PLIST_HEADER) pWireBuffer)->byteCount ;
		
		/* calculate the packed size of the data */
		ULONG packedByteCount = (localByteCount / sizeof (LIST_DATA)) * packed_sizeof_LIST_DATA ;
		
		/* write the packed size, incrementing the writePtr */
		ctx_wire_write32(&packedByteCount, writePtr) ; 

		/* point to the first list item */
		readPtr = pWireBuffer + sizeof (LIST_HEADER) ;
		
		while (localByteCount)
		{
			/* Write the data, incrementing the writePtr */
			/* Because of writePtr scope it is automatically kept to packed boundries */
			ctx_wire_write32(&readPtr->data, writePtr) ;

			/* point to the next list item, incrementing by the non packed size */
			readPtr += 1 ;
			
			/* decrement the loop counter by the non packed size */
			localByteCount -= sizeof (LIST_DATA) ;
		}

		/* return the total packed size */
		return (USHORT)(writePtr - pWireBuffer) ;
	}


#endif /* ImplementationNotes */
