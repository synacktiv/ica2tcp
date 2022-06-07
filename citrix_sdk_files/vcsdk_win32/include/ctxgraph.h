/******************************************************************************
 *
 *  ctxgraph.h
 *
 *  Graphics abstraction definitions.
 *
 * $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/ctxgraph.h#1 $
 *
 *  Copyright 1999-2015 Citrix Systems, Inc. 
 *
 *****************************************************************************/

#ifndef _CTXGRAPH_H_
#define _CTXGRAPH_H_

#pragma once

/*****************************************************************************
 *
 *  Error codes
 *
 *****************************************************************************/

#define GRAPH_SUCCESS		              0x00000000

#define GRAPH_MEM_SHARED                      0x00000800  /* GA internal */

#define GRAPH_ERR_NO_MEM                      0x80000001
#define GRAPH_ERR_INVALID_PARAMETER           0x80000002
#define GRAPH_ERR_OUT_OF_BOUNDS               0x80000003
#define GRAPH_ERR_NOT_PALETTED_SURFACE        0x80000004
#define GRAPH_ERR_PALETTE_ATTACHED            0x80000005
#define GRAPH_ERR_BUFFER_TOO_SMALL            0x80000006
#define GRAPH_ERR_CREATE_CRITICAL_SECTION     0x80000007
#define GRAPH_ERR_ENTER_CRITICAL_SECTION      0x80000008
#define GRAPH_ERR_LEAVE_CRITICAL_SECTION      0x80000009
#define GRAPH_ERR_DESTROY_CRITICAL_SECTION    0x8000000A
#define GRAPH_ERR_BAD_USER_HANDLE             0x8000000B
#define GRAPH_ERR_FREE_MEM                    0x8000000C
#define GRAPH_ERR_GDC_SHARING_VIOLATION       0x8000000D
#define GRAPH_ERR_PLATFORM_REGION_CREATE      0x8000000E
#define GRAPH_ERR_PLATFORM_REGION_DESTROY     0x8000000F
#define GRAPH_ERR_PLATFORM_REGION_GETBOX      0x80000010
#define GRAPH_ERR_PLATFORM_REGION_COMBINE     0x80000011
#define GRAPH_ERR_VALUE_NOT_SET               0x80000012
#define GRAPH_ERR_OS_API_FAILURE              0x80000013
#define GRAPH_ERR_ENUM_FAILED                 0x80000014
#define GRAPH_ERR_UNSUPPORTED                 0x80000015
#define GRAPH_ERR_INTERNAL_ERROR              0x80000016
#define GRAPH_ERR_PLATFORM_REGION_OFFSET      0x80000017
#define GRAPH_ERR_PLATFORM_REGION_STARTENUM   0x80000018
#define GRAPH_ERR_BAD_DEPTH                   0x80000019

#define GRAPH_ERR_NOT_IMPLEMENTED             0x8FFFFFFF

/*****************************************************************************
 *
 *  Data types
 *
 *****************************************************************************/

/*
 *  GA versioning:
 *
 *  1: minimal GA for TW1
 */

#define GA_VERSION  1

/*
 *  GA Capabilities
 */

/*
 *  !!!
 *  For version 1, ignore BEZIERS, ELLIPSES, LINE_ALG_NT, OFFSCREEN_SURF,
 *  and BITMAP2 (i.e. only VERSION, LINES, CURSOR, and BITMAP1 are valid).
 *  !!!
 */
#define GCAPINDEX_GA_VERSION        0   /* probly not necessary, but hey... */
#define GCAPINDEX_LINES             1   /* GDC_drawLines API available */
#define GCAPINDEX_BEZIERS           2   /* GDC_drawBeziers API available */
#define GCAPINDEX_ELLIPSES          3   /* GDC_drawEllipse API available  */
#define GCAPINDEX_CURSOR            4   /* what kind of cursor support */
#define GCAPINDEX_LINE_ALG_NT       5   /* GDC_drawLines supports NT line alg */
#define GCAPINDEX_OFFSCREEN_SURF    6   /* can draw on offscreen GDCs */
#define GCAPINDEX_BITMAP1           7   /* what kind of bitmaps are supported */
#define GCAPINDEX_BITMAP2           8   /* cont'd */
#define GCAPINDEX_OBSCUREDREGION    9   /* platform supports bitblt obscured regions */
#define GCAPINDEX_PALNOREDRAW       10  /* client do not need redraw after palette change*/

#define GCAPINDEX_MULTISIZE_BRUSH   11  /* support multisize brushes   */
#define GCAPINDEX_JPEG              12  /* support IJG JPEG library commands */
#define GCAPINDEX_MAX               13  /* keep this updated */


/* for most things */
#define GCAP_NOT_IMPLEMENTED        0x00
#define GCAP_IMPLEMENTED            0xFF

/* for GCAPINDEX_CURSOR */
#define GCAP_CURSOR_HARDWARE    1
#define GCAP_CURSOR_SOFTWARE    2

/* for GCAPINDEX_BITMAP1 */
#define GCAP_BITMAP_PALETTED_1BPP   0x01
#define GCAP_BITMAP_PALETTED_2BPP   0x02    /* IGNORE for version 1 */
#define GCAP_BITMAP_PALETTED_4BPP   0x04
#define GCAP_BITMAP_PALETTED_8BPP   0x08
#define GCAP_BITMAP_RGB_15BIT       0x10    /* IGNORE for version 1 */
#define GCAP_BITMAP_RGB_16BIT       0x20    /* IGNORE for version 1 */
#define GCAP_BITMAP_RGB_24BIT       0x40    /* IGNORE for version 1 */
#define GCAP_BITMAP_RGB_32BIT       0x80    /* IGNORE for version 1 */

/* for GCAPINDEX_BITMAP2 */
#define GCAP_BITMAP_YUV_24BIT       0x01    /* IGNORE for version 1 */

/* Bitmap creation modes: */
#define GA_BITMAP_CREATE_COMPATIBLE 0x00   /* Standard - compatible with the screen */
#define GA_BITMAP_CREATE_SECTION    0x01   /* Using DIB section (on Windows), used for accelerated data access */
#define GA_BITMAP_CREATE_LVB        0x02   /* let the platform code know if this bitmap is going to be used for an LVB */

/*
 *  Typedefs
 */

/* GAI object types - callers of GAI use these incomplete types as
 * handles, but internally GAI knows the internals of the structs.
 */
typedef struct _GDC *HGDC, FAR **PHGDC;
typedef struct _GBRUSH *HGBRUSH, FAR **PHGBRUSH;
typedef struct _GBITMAP *HGBITMAP, FAR **PHGBITMAP;
typedef struct _GCURSOR *HGCURSOR, FAR **PHGCURSOR;
typedef struct _GREGION *HGREGION, FAR **PHGREGION;


typedef enum
{
    GCS_UNSPECIFIED,
    GCS_PALETTED_1BIT,
    GCS_PALETTED_2BIT,
    GCS_PALETTED_4BIT,
    GCS_PALETTED_8BIT,
    GCS_RGB_15BIT,
    GCS_RGB_16BIT,
    GCS_RGB_24BIT,
    GCS_RGB_32BIT,     /* Native red-high RGB 32-bit format */
    GCS_RGB_16BIT_565, /* Native red-high RGB 16-bit format */
	GCS_YUV_420,
	GCS_YUV_422,
	GCS_YUV_444_SWAPPED_RB,
	GCS_YUV_444_MAINTAINED_RB
} GRAPH_COLORSPACE;

/* This define may need updating if GRAPH_COLORSPACE is changed */
#define NUM_GRAPH_COLORSPACE_ENUMS (GCS_RGB_16BIT_565 + 1)

#define COLOR_INVALID    0x00
#define COLOR_RGB        0x01
#define COLOR_YUV        0x02               /* IGNORE for version 1 */
#define COLOR_PALETTEREF 0x03
#define COLOR_IDENTITYPAL 0x04

typedef struct
{
    UINT8 colorData[3];
    UINT8 flags;
} GRAPH_COLOR, FAR * PGRAPH_COLOR;

typedef struct
{
    INT32 width;
    INT32 height;
    GRAPH_COLORSPACE colorSpace;
    PGRAPH_COLOR vColors;
} GRAPH_BITMAPINFO, FAR * PGRAPH_BITMAPINFO;


typedef struct _GRAPH_LVBINFO
{
    BOOL                lvbExists;
    INT32               width;
    INT32               height;
    GRAPH_COLORSPACE    colorSpace;
    INT32               bitsPerPixel;
    PBYTE               pFirstPixel;
    INT32               stride;
} GRAPH_LVBINFO, *PGRAPH_LVBINFO;


typedef struct
{
    INT32 xLeft;
    INT32 yTop;
    INT32 xRight;
    INT32 yBottom;
} GRAPH_RECT, FAR * PGRAPH_RECT;


typedef struct
{
    INT32 resX;
    INT32 resY;
    GRAPH_COLORSPACE colorSpace;
} GRAPH_MODEINFO, FAR * PGRAPH_MODEINFO;

typedef enum
{
    ROP2_CLEAR,                 /* 0          */
    ROP2_SRC,                   /* S          */
    ROP2_DST,                   /* D          */
    ROP2_NOTDST,                /* ~D         */
    ROP2_SRC_AND_DST,           /* S & D      */
    ROP2_SRC_AND_NOTDST,        /* S & ~D     */
    ROP2_NOTSRC_AND_DST,        /* ~S & D     */
    ROP2_SRC_OR_DST,            /* S | D      */
    ROP2_SRC_OR_NOTDST,         /* S | ~D     */
    ROP2_NOTSRC_OR_DST,         /* ~S | D     */
    ROP2_SRC_XOR_DST,           /* S xor D    */
    ROP2_NOT_SRC,               /* ~(S)       */
    ROP2_NOT_SRC_AND_DST,       /* ~(S & D)   */
    ROP2_NOT_SRC_OR_DST,        /* ~(S | D)   */
    ROP2_NOT_SRC_XOR_DST,       /* ~(S xor D) */
    ROP2_SET,                    /* 1          */
    ROP2_INVALID
} GRAPH_ROP2, *PGRAPH_ROP2;



typedef UINT8 GRAPH_ROP3;

/*
 *  Give names to some particularly useful ROP3s.
 */
#define GRP_SRCCOPY     (GRAPH_ROP3)0xCC /* dest = source                   */
#define GRP_SRCPAINT    (GRAPH_ROP3)0xEE /* dest = source OR dest           */
#define GRP_SRCAND      (GRAPH_ROP3)0x88 /* dest = source AND dest          */
#define GRP_SRCINVERT   (GRAPH_ROP3)0x66 /* dest = source XOR dest          */
#define GRP_SRCERASE    (GRAPH_ROP3)0x44 /* dest = source AND (NOT dest )   */
#define GRP_NOTSRCCOPY  (GRAPH_ROP3)0x33 /* dest = (NOT source)             */
#define GRP_NOTSRCERASE (GRAPH_ROP3)0x11 /* dest = (NOT src) AND (NOT dest) */
#define GRP_MERGECOPY   (GRAPH_ROP3)0xC0 /* dest = (source AND pattern)     */
#define GRP_MERGEPAINT  (GRAPH_ROP3)0xBB /* dest = (NOT source) OR dest     */
#define GRP_PATCOPY     (GRAPH_ROP3)0xF0 /* dest = pattern                  */
#define GRP_PATPAINT    (GRAPH_ROP3)0xFB /* dest = DPSnoo                   */
#define GRP_PATINVERT   (GRAPH_ROP3)0x5A /* dest = pattern XOR dest         */
#define GRP_DSTINVERT   (GRAPH_ROP3)0x55 /* dest = (NOT dest)               */
#define GRP_BLACKNESS   (GRAPH_ROP3)0x00 /* dest = BLACK                    */
#define GRP_WHITENESS   (GRAPH_ROP3)0xFF /* dest = WHITE                    */


typedef enum
{
    REGION_OP_UNION,
    REGION_OP_DIFF,
    REGION_OP_INTERSECT,
    REGION_OP_XOR,
    REGION_OP_COPY
} GRAPH_REGION_OP;

typedef USHORT GRAPH_COLOR16;

typedef struct 
{
    LONG    x;
    LONG    y;
    GRAPH_COLOR16 Red;
    GRAPH_COLOR16 Green;
    GRAPH_COLOR16 Blue;
} GRAPH_TRIVERTEX, *PGRAPH_TRIVERTEX;

typedef BOOLEAN
(FAR * PFNENUMMODESCALLBACK)( PGRAPH_MODEINFO, PVOID );


/*
 * Below definitions are used with CALLBACK FUNCTIONS that are set
 * through the Graph_setCallbacks function.
 *
 * Part of the definition is that the caller of Graph_setCallbacks() provides
 * a pointer to persistent memory (static or dynamically allocated) so that
 * the GA does need not (and should not) copy the structure. That allows the
 * caller to change the functions at any time.
 *
 * OSS - here means off screen surfaces (also known as in-memory bitmaps)
 *
 */

typedef BOOLEAN (FAR * PFNOSSCALLBACK)(HGDC, PVOID);


typedef struct _ga_callbacks {
 	PFNOSSCALLBACK	OnBitmapCreateFailed;
 	PFNOSSCALLBACK	OnBitmapIsDirty;
} GRAPH_CALLBACKS, *PGRAPH_CALLBACKS;




/****************************************************************************
 * Callback function definition that is used for the JPEG functionality     *
 * provided by the GAI                                                      *
 *                                                                          *
 * Description                                                              *
 *                                                                          *
 * A function pointer to a callback function with this prototype can be     *
 * passed into the GAI JPEG decompression functions.  When the libJPEG JPEG *
 * engine requires more data, the callback function provided by the caller  *
 * will be invoked.  The caller will return through the parameters of the   *
 * function a pointer to the additional JPEG data, and the length of that   *
 * data.  The additional JPEG data will be sequential with the data already *
 * passed into the function.  This function may be called many times during *
 * the decompression of a single image.  The number of times that the       *
 * function is invoked is dependant on the amount of data returned to GAI   *
 * with each invocation of this function.                                   *
 *                                                                          *
 * Parameters                                                               *
 *                                                                          *
 * ppBuffer: Out parameter which receives a pointer to additional JPEG data.*
 *                                                                          *
 * pByteCount: Pointer to UINT32 which receives the length of the buffer    *
 * pointed at by the pointer returned through the ppBuffer parameter.       *
 ***************************************************************************/

typedef BOOL (_VPAPI * PFNSOURCEFILLBUFFER) (PVOID * pContext, PBYTE * ppBuffer,PUINT32 pByteCount);

typedef enum CTXCODEC
{
	UNSUPPORTED_CODEC = -1,
	NULL_CODEC = 0,
	RLE_V1_CODEC,
	RLE_V2_CODEC,
    JPEG_CODEC,
    PEGASUS_IMAGE_CODEC,
    HDPHOTO_CODEC,
	H264_CODEC,
	H265_CODEC,
	MDRLE_CODEC
}CTXCODEC , * PCTXCODEC;

typedef struct BITMAP_ENCODED_DATA
{
    PBYTE pEncodedData;
    UINT32 EncodedDataSize;
    CTXCODEC Codec;
	UINT32 CodecFlags;
}BITMAP_ENCODED_DATA, *PBITMAP_ENCODED_DATA;

typedef struct BITMAP_DECODER_CALLBACK
{
    PFNSOURCEFILLBUFFER pfnSourceFillBuffer;
    PVOID               pContext;
}BITMAP_DECODER_CALLBACK, *PBITMAP_DECODER_CALLBACK;


typedef struct BITMAP_DECODE_DATA
{
    PBYTE               pBitmapBits;
    UINT32              BitmapBitSize;
    PGRAPH_BITMAPINFO   pBitmapInfo;
    PUINT               pDesiredBitsPerPixel;
}BITMAP_DECODE_DATA, *PBITMAP_DECODE_DATA;
#define MAX_DIRTY_RECT 31 //from protocol spec
typedef struct IMAGE_REGION_DATA
{    
    UINT8              RectCount;
    //Sharpening Context
    UINT8               ImageQ;
    GRAPH_RECT          DirtyRect[MAX_DIRTY_RECT];
}IMAGE_REGION_DATA, *PIMAGE_REGION_DATA;

/*****************************************************************************
 *
 *  Function types for vtable
 *
 *****************************************************************************/
typedef VPSTATUS (_PVPAPI PFNGRAPH_GETDISPLAYMODE)(PGRAPH_MODEINFO);
typedef VPSTATUS (_PVPAPI PFNGRAPH_SETDISPLAYMODE)(PGRAPH_MODEINFO);
typedef VPSTATUS (_PVPAPI PFNGRAPH_ENUMDISPLAYMODES)(PFNENUMMODESCALLBACK,
                                                     PVOID);
typedef VPSTATUS (_PVPAPI PFNGRAPH_GETCAPABILITIES)( PUINT16, PUINT8 );
typedef VPSTATUS (_PVPAPI PFNGDC_FLUSH)(HGDC);
typedef VPSTATUS (_PVPAPI PFNGDC_CLONE)(HGDC, PHGDC);

typedef VPSTATUS (_PVPAPI PFNGDC_REALIZEPALETTE)(HGDC);
typedef VPSTATUS (_PVPAPI PFNGDC_CREATE)(HND, PHGDC);
typedef VPSTATUS (_PVPAPI PFNGDC_CREATECOMPATIBLE)(HGDC, PHGDC);
typedef VPSTATUS (_PVPAPI PFNGDC_DESTROY)(PHGDC);
typedef VPSTATUS (_PVPAPI PFNGDC_SELECTBITMAP)(HGDC, HGBITMAP);
typedef VPSTATUS (_PVPAPI PFNGDC_SELECTCLIPREGION)(HGDC, HGREGION);
typedef VPSTATUS (_PVPAPI PFNGDC_SELECTBRUSH)(HGDC, HGBRUSH);
typedef VPSTATUS (_PVPAPI PFNGDC_SETBRUSHORIGIN)(HGDC, INT32, INT32);
typedef VPSTATUS (_PVPAPI PFNGDC_SETPALETTE)(HGDC, UINT8, UINT16, PGRAPH_COLOR, PBOOL);
typedef VPSTATUS (_PVPAPI PFNGDC_SETFGCOLOR)(HGDC, GRAPH_COLOR);
typedef VPSTATUS (_PVPAPI PFNGDC_GETFGCOLOR)(HGDC, PGRAPH_COLOR);
typedef VPSTATUS (_PVPAPI PFNGDC_SETBKCOLOR)(HGDC, GRAPH_COLOR);
typedef VPSTATUS (_PVPAPI PFNGDC_GETBKCOLOR)(HGDC, PGRAPH_COLOR);
typedef VPSTATUS (_PVPAPI PFNGDC_SETROP2)(HGDC, GRAPH_ROP2);
typedef VPSTATUS (_PVPAPI PFNGDC_GETROP2)(HGDC, PGRAPH_ROP2);
typedef VPSTATUS (_PVPAPI PFNGDC_BITBLT)(HGDC, INT32, INT32, INT32, INT32,
                                         HGDC, INT32, INT32, GRAPH_ROP3, PHGREGION);
typedef VPSTATUS (_PVPAPI PFNGDC_PAINTREGION)(HGDC, HGREGION);
typedef VPSTATUS (_PVPAPI PFNGDC_FILLRECT)(HGDC, PGRAPH_RECT);
typedef VPSTATUS (_PVPAPI PFNGDC_MOVETO)(HGDC, INT32, INT32);
typedef VPSTATUS (_PVPAPI PFNGDC_LINETO)(HGDC, INT32, INT32);
typedef VPSTATUS (_PVPAPI PFNGDC_GETBITS)(HGDC, PGRAPH_RECT,
                                          PGRAPH_BITMAPINFO, PVOID);
typedef VPSTATUS (_PVPAPI PFNGDC_SETBITS)(HGDC, INT32, INT32, INT32, INT32,
                                          INT32, INT32, PVOID, PGRAPH_BITMAPINFO, GRAPH_ROP3);




typedef VPSTATUS (_PVPAPI PFNGDC_STRETCHBITS)(HGDC, INT32, INT32, INT32, INT32,
                                          INT32, INT32, INT32, INT32, PVOID, PGRAPH_BITMAPINFO, GRAPH_ROP3);
typedef VPSTATUS (_PVPAPI PFNGRAPH_GRADIENTFILL) (HGDC, PGRAPH_TRIVERTEX, UINT32, PUINT32, UINT32, UINT32);
typedef VPSTATUS (_PVPAPI PFNGDC_GETCLIENTID)(HGDC, PUINT32);

typedef VPSTATUS (_PVPAPI PFNBRUSH_CREATE) (HGDC, PGRAPH_BITMAPINFO, PVOID, PHGBRUSH);
typedef VPSTATUS (_PVPAPI PFNBRUSH_CREATESOLID)(GRAPH_COLOR, PHGBRUSH);
typedef VPSTATUS (_PVPAPI PFNBRUSH_DESTROY)(PHGBRUSH);
typedef VPSTATUS (_PVPAPI PFNBRUSH_DUPLICATE) (HGBRUSH, PHGBRUSH);
typedef VPSTATUS (_PVPAPI PFNBITMAP_CREATE)(HGDC, PGRAPH_BITMAPINFO, PVOID,
                                            PHGBITMAP, UINT32);
typedef VPSTATUS (_PVPAPI PFNBITMAP_CREATEFROMJPEG) (PBYTE, UINT32, PFNSOURCEFILLBUFFER,PVOID,PGRAPH_BITMAPINFO,PBYTE*, PUINT32);
typedef VPSTATUS (_PVPAPI PFNBITMAP_QUERYJPEGINFORMATION)(PBYTE,UINT32,PFNSOURCEFILLBUFFER,PVOID,PGRAPH_BITMAPINFO,PUINT32);

typedef BOOL     (_PVPAPI PFNBITMAP_SUPPORTSENCODEDTYPE)(CTXCODEC);
typedef VPSTATUS (_PVPAPI PFNBITMAP_CREATEFROMENCODED)(PBITMAP_ENCODED_DATA, PBITMAP_DECODER_CALLBACK,PBITMAP_DECODE_DATA);
typedef VPSTATUS (_PVPAPI PFNBITMAP_QUERYENCODEDINFORMATION)(PBITMAP_ENCODED_DATA, PUINT,PUINT32);

typedef VPSTATUS (_PVPAPI PFNBITMAP_DESTROY)(PHGBITMAP);

typedef VPSTATUS (_PVPAPI PFNCURSOR_CREATE)( INT32, INT32 ,PVOID, PGRAPH_BITMAPINFO,
                                PVOID, PGRAPH_BITMAPINFO, BOOLEAN, HGDC ,PHGCURSOR );
typedef VPSTATUS (_PVPAPI PFNCURSOR_CREATESCALED)( INT32, INT32 ,PVOID, PGRAPH_BITMAPINFO,
                                PVOID, PGRAPH_BITMAPINFO, BOOLEAN, HGDC ,PHGCURSOR, UINT32, BOOLEAN );
typedef VPSTATUS (_PVPAPI PFNCURSOR_DESTROY)(PHGCURSOR);

typedef VPSTATUS (_PVPAPI PFNREGION_CREATE)(UINT16, PGRAPH_RECT, PHGREGION);
typedef VPSTATUS (_PVPAPI PFNREGION_DESTROY)(PHGREGION);
typedef VPSTATUS (_PVPAPI PFNREGION_COMBINE)(HGREGION, HGREGION,
                                             GRAPH_REGION_OP, HGREGION);
typedef VPSTATUS (_PVPAPI PFNREGION_GETBOX)(HGREGION, PGRAPH_RECT);
typedef VPSTATUS (_PVPAPI PFNREGION_OFFSET)(HGREGION, INT32, INT32);
typedef VPSTATUS (_PVPAPI PFNREGION_STARTENUM)(HGREGION, PPVOID);
typedef BOOL     (_PVPAPI PFNREGION_ENUM)(PVOID, PGRAPH_RECT);
typedef VPSTATUS (_PVPAPI PFNREGION_ENDENUM)(PVOID);
typedef VPSTATUS (_PVPAPI PFNGDC_GETUSERDATA)(HGDC, PPVOID);
typedef VPSTATUS (_PVPAPI PFNGDC_SETUSERDATA)(HGDC, PVOID);
typedef VPSTATUS (_PVPAPI PFNGRAPH_SETCALLBACKS)( GRAPH_CALLBACKS * );

typedef VPSTATUS (_PVPAPI PFNGRAPH_GETFONTSMOOTHING)(PUSHORT);

typedef VPSTATUS (_PVPAPI PFNDECODE_H264)(
    PUINT8 pEncodeBuffer,
    UINT32 encodeBufferSize,
    GRAPH_COLORSPACE encodeColorSpace,
    PUINT8 pDecodeBuffer,
    UINT32 decodeBufferStride,
    GRAPH_COLORSPACE decodeColorSpace,
    PGRAPH_RECT pContextRect,
    PGRAPH_RECT pDirtyRects,
    UINT8 dirtyRectCount);

typedef VPSTATUS(_PVPAPI PFNCLEANUP_H264)(VOID);

typedef VPSTATUS(_PVPAPI PFNCLEANUP_H264_CONTEXT)(PGRAPH_RECT);

/*****************************************************************************
 *
 *  Vtable type
 *
 *****************************************************************************/
typedef struct _GRAPHCALLTABLE
{
    PFNGRAPH_GETDISPLAYMODE     pfnGraph_getDisplayMode;
    PFNGRAPH_SETDISPLAYMODE     pfnGraph_setDisplayMode;
    PFNGRAPH_ENUMDISPLAYMODES   pfnGraph_enumDisplayModes;
    PFNGRAPH_GETCAPABILITIES    pfnGraph_getCapabilities;
    PFNGDC_FLUSH                pfnGDC_flush;
    PFNGDC_CLONE                pfnGDC_clone;
    PFNGDC_REALIZEPALETTE       pfnGDC_realizePalette;
    PFNGDC_CREATE               pfnGDC_create;
    PFNGDC_CREATECOMPATIBLE     pfnGDC_createCompatible;
    PFNGDC_DESTROY              pfnGDC_destroy;
    PFNGDC_SELECTBITMAP         pfnGDC_selectBitmap;
    PFNGDC_SELECTCLIPREGION     pfnGDC_selectClipRegion;
    PFNGDC_SELECTBRUSH          pfnGDC_selectBrush;
    PFNGDC_SETBRUSHORIGIN       pfnGDC_setBrushOrigin;
    PFNGDC_SETPALETTE           pfnGDC_setPalette;
    PFNGDC_SETFGCOLOR           pfnGDC_setFgColor;
    PFNGDC_GETFGCOLOR           pfnGDC_getFgColor;
    PFNGDC_SETBKCOLOR           pfnGDC_setBkColor;
    PFNGDC_GETBKCOLOR           pfnGDC_getBkColor;
    PFNGDC_SETROP2              pfnGDC_setROP2;
    PFNGDC_GETROP2              pfnGDC_getROP2;
    PFNGDC_BITBLT               pfnGDC_bitBlt;
    PFNGDC_PAINTREGION          pfnGDC_paintRegion;
    PFNGDC_FILLRECT             pfnGDC_fillRect;
    PFNGDC_MOVETO               pfnGDC_moveTo;
    PFNGDC_LINETO               pfnGDC_lineTo;
    PFNGDC_GETBITS              pfnGDC_getBits;
    PFNGDC_SETBITS              pfnGDC_setBits;
    PFNGDC_STRETCHBITS          pfnGDC_stretchBits;
    PFNGRAPH_GRADIENTFILL       pfnGDC_gradientFill;
    PFNBRUSH_CREATE             pfnBrush_create;
    PFNBRUSH_CREATESOLID        pfnBrush_createSolid;
    PFNBRUSH_DESTROY            pfnBrush_destroy;
    PFNBRUSH_DUPLICATE          pfnBrush_duplicateSolidBrush;
    PFNBITMAP_CREATE            pfnBitmap_create;
    PFNBITMAP_DESTROY           pfnBitmap_destroy;
    PFNCURSOR_CREATE            pfnCursor_create;
    PFNCURSOR_DESTROY           pfnCursor_destroy;
    PFNREGION_CREATE            pfnRegion_create;
    PFNREGION_DESTROY           pfnRegion_destroy;
    PFNREGION_COMBINE           pfnRegion_combine;
    PFNREGION_GETBOX            pfnRegion_getBox;
    PFNREGION_OFFSET            pfnRegion_offset;
    PFNREGION_STARTENUM         pfnRegion_startEnum;
    PFNREGION_ENUM              pfnRegion_enum;
    PFNREGION_ENDENUM           pfnRegion_endEnum;
    PFNGDC_GETUSERDATA          pfnGDC_getUserData;
    PFNGDC_SETUSERDATA          pfnGDC_setUserData;
    PFNGRAPH_SETCALLBACKS       pfnGraph_setCallbacks;
    PFNGRAPH_GETFONTSMOOTHING   pfnGraph_getFontSmoothing;
    PFNBITMAP_CREATEFROMJPEG    pfnBitmap_createFromJPEG;
    PFNBITMAP_QUERYJPEGINFORMATION      pfnBitmap_queryJPEGInformation;
    PFNBITMAP_SUPPORTSENCODEDTYPE       pfnBitmap_supportsEncodedType;
    PFNBITMAP_CREATEFROMENCODED         pfnBitmap_createFromEncoded;
    PFNBITMAP_QUERYENCODEDINFORMATION   pfnBitmap_queryEncodedInformation;
    PFNGDC_GETCLIENTID          pfnGDC_getClientId;
    PFNDECODE_H264              pfnDecodeH264;
    PFNCLEANUP_H264             pfnCleanupH264;
    PFNCLEANUP_H264_CONTEXT     pfnCleanupH264Context;
    PFNCURSOR_CREATESCALED      pfnCursor_createScaled;
} GRAPHCALLTABLE, FAR * PGRAPHCALLTABLE, FAR * FAR * PPGRAPHCALLTABLE;

#define GRAPH_STRING "CTXGRAPH"

VPSTATUS _VPAPI Graph_load();
VPSTATUS _VPAPI Graph_unload();



/*****************************************************************************
 *
 *  Macros for accessing the vtable
 *
 *****************************************************************************/
#define GRAPH_GLOBAL_INIT PGRAPHCALLTABLE g_pGraphCallTable = NULL;
#define GRAPH_EXTERN_INIT extern PGRAPHCALLTABLE g_pGraphCallTable;

#ifdef USE_GRAPH_CALL_TABLE

#define GraphRunTimeInit(pLibMgr, pVer) \
    ((*pLibMgr->pLMgrUse)(GRAPH_STRING,pVer,(PPLMGRTABLE)&g_pGraphCallTable))
#define GraphRunTimeRelease(pLibMgr) \
    ((*pLibMgr->pLMgrRelease)((PLMGRTABLE)g_pGraphCallTable))

GRAPH_EXTERN_INIT

#define Graph_getDisplayMode (*(g_pGraphCallTable)->pfnGraph_getDisplayMode)
#define Graph_setDisplayMode (*(g_pGraphCallTable)->pfnGraph_setDisplayMode)
#define Graph_enumDisplayModes (*(g_pGraphCallTable)->pfnGraph_enumDisplayModes)
#define Graph_getCapabilities (*(g_pGraphCallTable)->pfnGraph_getCapabilities)
#define GDC_flush (*(g_pGraphCallTable)->pfnGDC_flush)
#define GDC_clone (*(g_pGraphCallTable)->pfnGDC_clone)
#define GDC_realizePalette (*(g_pGraphCallTable)->pfnGDC_realizePalette)
#define GDC_create (*(g_pGraphCallTable)->pfnGDC_create)
#define GDC_createCompatible (*(g_pGraphCallTable)->pfnGDC_createCompatible)
#define GDC_destroy (*(g_pGraphCallTable)->pfnGDC_destroy)
#define GDC_selectBitmap (*(g_pGraphCallTable)->pfnGDC_selectBitmap)
#define GDC_selectClipRegion (*(g_pGraphCallTable)->pfnGDC_selectClipRegion)
#define GDC_selectBrush (*(g_pGraphCallTable)->pfnGDC_selectBrush)
#define GDC_setBrushOrigin (*(g_pGraphCallTable)->pfnGDC_setBrushOrigin)
#define GDC_setPalette (*(g_pGraphCallTable)->pfnGDC_setPalette)
#define GDC_setFgColor (*(g_pGraphCallTable)->pfnGDC_setFgColor)
#define GDC_getFgColor (*(g_pGraphCallTable)->pfnGDC_getFgColor)
#define GDC_setBkColor (*(g_pGraphCallTable)->pfnGDC_setBkColor)
#define GDC_getBkColor (*(g_pGraphCallTable)->pfnGDC_getBkColor)
#define GDC_setROP2 (*(g_pGraphCallTable)->pfnGDC_setROP2)
#define GDC_getROP2 (*(g_pGraphCallTable)->pfnGDC_getROP2)
#define GDC_bitBlt (*(g_pGraphCallTable)->pfnGDC_bitBlt)
#define GDC_paintRegion (*(g_pGraphCallTable)->pfnGDC_paintRegion)
#define GDC_fillRect (*(g_pGraphCallTable)->pfnGDC_fillRect)
#define GDC_moveTo (*(g_pGraphCallTable)->pfnGDC_moveTo)
#define GDC_lineTo (*(g_pGraphCallTable)->pfnGDC_lineTo)
#define GDC_getBits (*(g_pGraphCallTable)->pfnGDC_getBits)
#define GDC_setBits (*(g_pGraphCallTable)->pfnGDC_setBits)
#define GDC_stretchBits (*(g_pGraphCallTable)->pfnGDC_stretchBits)
#define GDC_gradientFill (*(g_pGraphCallTable)->pfnGDC_gradientFill)
#define Brush_create (*(g_pGraphCallTable)->pfnBrush_create)
#define Brush_createSolid (*(g_pGraphCallTable)->pfnBrush_createSolid)
#define Brush_destroy (*(g_pGraphCallTable)->pfnBrush_destroy)
#define Brush_duplicateSolidBrush (*(g_pGraphCallTable)->pfnBrush_duplicateSolidBrush)
#define Bitmap_create (*(g_pGraphCallTable)->pfnBitmap_create)
#define Bitmap_destroy (*(g_pGraphCallTable)->pfnBitmap_destroy)
#define Cursor_create (*(g_pGraphCallTable)->pfnCursor_create)
#define Cursor_destroy (*(g_pGraphCallTable)->pfnCursor_destroy)
#define Region_create (*(g_pGraphCallTable)->pfnRegion_create)
#define Region_destroy (*(g_pGraphCallTable)->pfnRegion_destroy)
#define Region_combine (*(g_pGraphCallTable)->pfnRegion_combine)
#define Region_getBox (*(g_pGraphCallTable)->pfnRegion_getBox)
#define Region_offset (*(g_pGraphCallTable)->pfnRegion_offset)
#define Region_startEnum (*(g_pGraphCallTable)->pfnRegion_startEnum)
#define Region_enum (*(g_pGraphCallTable)->pfnRegion_enum)
#define Region_endEnum (*(g_pGraphCallTable)->pfnRegion_endEnum)
#define GDC_getUserData (*(g_pGraphCallTable)->pfnGDC_getUserData)
#define GDC_setUserData (*(g_pGraphCallTable)->pfnGDC_setUserData)
#define Graph_setCallbacks (*(g_pGraphCallTable)->pfnGraph_setCallbacks)
#define Bitmap_createFromJPEG (*(g_pGraphCallTable)->pfnBitmap_createFromJPEG)
#define Bitmap_queryJPEGInformation (*(g_pGraphCallTable)->pfnBitmap_queryJPEGInformation)
#define Bitmap_supportsEncodedType (*(g_pGraphCallTable)->pfnBitmap_supportsEncodedType)
#define Bitmap_createFromEncoded (*(g_pGraphCallTable)->pfnBitmap_createFromEncoded)
#define Bitmap_queryEncodedInformation (*(g_pGraphCallTable)->pfnBitmap_queryEncodedInformation)
#define Graph_getFontSmoothing (*(g_pGraphCallTable)->pfnGraph_getFontSmoothing)
#define GDC_getClientId (*(g_pGraphCallTable)->pfnGDC_getClientId)
#define DecodeH264 (*(g_pGraphCallTable)->pfnDecodeH264)
#define CleanupH264 (*(g_pGraphCallTable)->pfnCleanupH264)
#define CleanupH264Context (*(g_pGraphCallTable)->pfnCleanupH264Context)
#define Cursor_createScaled (*(g_pGraphCallTable)->pfnCursor_createScaled)

#else /* USE_GRAPH_CALL_TABLE */
/*****************************************************************************
 *
 *  Prototypes for when we aren't using the vtable (e.g. GA implementations)
 *
 *****************************************************************************/

#ifdef IN
#undef IN
#endif
#define IN

#ifdef OUT
#undef OUT
#endif
#define OUT

#ifdef INOUT
#undef INOUT
#endif
#define INOUT

#define GraphRunTimeInit(pLibMgr, pVer)
#define GraphRunTimeRelease(pLibMgr)

VPSTATUS
_VPAPI
Graph_getDisplayMode(
    OUT PGRAPH_MODEINFO pModeInfo );

VPSTATUS
_VPAPI
Graph_setDisplayMode(
    IN  PGRAPH_MODEINFO pModeInfo );

VPSTATUS
_VPAPI
Graph_enumDisplayModes(
    IN  PFNENUMMODESCALLBACK pfnEnumModesCallback,
    IN  PVOID pUserData );

VPSTATUS
_VPAPI
Graph_getCapabilities(
    INOUT PUINT16 pcbCaps,
    OUT   PUINT8 pCaps );

VPSTATUS
_VPAPI
GDC_flush(
    INOUT HGDC hGDC);

VPSTATUS
_VPAPI
GDC_clone(
    IN  HGDC hGDC_Orig,
    OUT PHGDC phGDC_Clone);

VPSTATUS
_VPAPI
GDC_realizePalette(
    IN HGDC hGDC);

VPSTATUS
_VPAPI
GDC_create(
    IN   HND hWindow,
    OUT  PHGDC pNewHGDC );

VPSTATUS
_VPAPI
GDC_createCompatible(
    IN   HGDC  hGDC,
    OUT  PHGDC pNewHGDC );

VPSTATUS
_VPAPI
GDC_destroy(
    IN  PHGDC phGDC );

VPSTATUS
_VPAPI
GDC_selectBitmap(
    IN  HGDC hGDC,
    IN  HGBITMAP hBitmap );

VPSTATUS
_VPAPI
GDC_selectClipRegion(
    IN  HGDC hGDC,
    IN  HGREGION hRegion );

VPSTATUS
_VPAPI
GDC_selectBrush(
    IN  HGDC hGDC,
    IN  HGBRUSH hBrush );

VPSTATUS
_VPAPI
GDC_setBrushOrigin(
    IN   HGDC hGDC,
    IN   INT32 newX,
    IN   INT32 newY );

VPSTATUS
_VPAPI
GDC_setPalette(
    IN    HGDC hGDC,
    IN    UINT8 startEntry,
    IN    UINT16 numEntries,
    IN    PGRAPH_COLOR vColors,
    INOUT PBOOL pbRedraw);

VPSTATUS
_VPAPI
GDC_setFgColor(
    IN  HGDC hGDC,
    IN  GRAPH_COLOR color );

VPSTATUS
_VPAPI
GDC_getFgColor(
    IN  HGDC hGDC,
    OUT PGRAPH_COLOR pColor );

VPSTATUS
_VPAPI
GDC_setBkColor(
    IN  HGDC hGDC,
    IN  GRAPH_COLOR color );

VPSTATUS
_VPAPI
GDC_getBkColor(
    IN  HGDC hGDC,
    OUT PGRAPH_COLOR pColor );

VPSTATUS
_VPAPI
GDC_setROP2(
    IN  HGDC hGDC,
    IN  GRAPH_ROP2 rop );

VPSTATUS
_VPAPI
GDC_getROP2(
IN  HGDC hGDC ,
OUT PGRAPH_ROP2 pROP2
);

VPSTATUS
_VPAPI
GDC_bitBlt(
    IN  HGDC hDstGDC,
    IN  INT32 destX,
    IN  INT32 destY,
    IN  INT32 destWidth,
    IN  INT32 destHeight,
    IN  HGDC hSrcGDC,
    IN  INT32 srcX,
    IN  INT32 srcY,
    IN  GRAPH_ROP3 rop,
    INOUT PHGREGION phObscuredRegion );

VPSTATUS
_VPAPI
GDC_paintRegion(
    IN   HGDC hGDC,
    IN   HGREGION hRegion );

VPSTATUS
_VPAPI
GDC_fillRect(
    IN   HGDC hGDC,
    IN   PGRAPH_RECT pRect );

VPSTATUS
_VPAPI
GDC_moveTo(
    IN   HGDC hGDC,
    IN   INT32 newX,
    IN   INT32 newY );

VPSTATUS
_VPAPI
GDC_lineTo(
    IN   HGDC hGDC,
    IN   INT32 endX,
    IN   INT32 endY );

VPSTATUS
_VPAPI
GDC_getBits(
    IN    HGDC hGDC,
    IN    PGRAPH_RECT pSrcRect,
    INOUT PGRAPH_BITMAPINFO pGBitmapInfo,
    OUT   PVOID pData);

VPSTATUS
_VPAPI
GDC_setBits(
    IN   HGDC dest,
    IN   INT32 destX,
    IN   INT32 destY,
    IN   INT32 Width,
    IN   INT32 Height,
    IN   INT32 srcX,
    IN   INT32 srcY,
    IN   PVOID pData,
    IN   PGRAPH_BITMAPINFO pGBitmapInfo,
    IN   GRAPH_ROP3 rop );

VPSTATUS
_VPAPI
GDC_stretchBits(
    IN   HGDC dest,
    IN   INT32 destX,
    IN   INT32 destY,
    IN   INT32 destWidth,
    IN   INT32 destHeight,
    IN   INT32 srcX,
    IN   INT32 srcY,
    IN   INT32 srcWidth,
    IN   INT32 srcHeight,
    IN   PVOID pData,
    IN   PGRAPH_BITMAPINFO pGBitmapInfo,
    IN   GRAPH_ROP3 rop );

VPSTATUS
_VPAPI
GDC_gradientFill(
    IN   HGDC dest,
    IN   PGRAPH_TRIVERTEX pVertex,
    IN   UINT32 nVertex,
    IN   PUINT32 pMesh,
    IN   UINT32 nMesh,
    IN   UINT32 mode
    );

VPSTATUS
_VPAPI
GDC_getClientId(
    IN HGDC hGDC,
    OUT PUINT32 pClientId);

VPSTATUS
_VPAPI
Brush_create(
    IN   HGDC hgdc,
    IN   PGRAPH_BITMAPINFO pBitmapInfo,
    IN   PVOID pBitmapData,
    OUT  PHGBRUSH phBrush );

VPSTATUS
_VPAPI
Brush_createSolid(
    IN   GRAPH_COLOR color,
    OUT  PHGBRUSH phBrush );

VPSTATUS
_VPAPI
Brush_destroy(
    IN   PHGBRUSH phBrush );

VPSTATUS
_VPAPI
Brush_duplicateSolidBrush(
    IN   HGBRUSH  SourceBrush,
    OUT  PHGBRUSH pDestinationBrush );

VPSTATUS
_VPAPI
Bitmap_create(
    IN   HGDC hgdc,
    IN   PGRAPH_BITMAPINFO pBitmapInfo,
    IN   PVOID pBitmapData,
    OUT  PHGBITMAP phBitmap,
	IN   UINT32  flags);

VPSTATUS
_VPAPI
Bitmap_destroy(
    IN   PHGBITMAP phBitmap );

VPSTATUS
_VPAPI
Bitmap_createFromJPEG(
    PBYTE               pJpegData,
    UINT32               jpegDataSize,
    PFNSOURCEFILLBUFFER pfnSourceFillBuffer,
    PVOID               pContext,
    PGRAPH_BITMAPINFO   pBitmapInfo,
    PBYTE  *            ppBitmapBits,
    PUINT32             pBitmapBitSize);

VPSTATUS
_VPAPI
Bitmap_queryJPEGInformation(
    PBYTE               pJpegData,
    UINT32              jpegDataSize,
    PFNSOURCEFILLBUFFER pfnSourceFillBuffer,
    PVOID               pContext,
    PGRAPH_BITMAPINFO   pBitmapInfo,
    PUINT32             pDecompressedByteCount);

BOOL _VPAPI Bitmap_supportsEncodedType(CTXCODEC CodecType);

VPSTATUS _VPAPI DecodeH264(
    PUINT8 pEncodeBuffer,
    UINT32 encodeBufferSize,
    GRAPH_COLORSPACE encodeColorFormat,
    PUINT8 pDecodeBuffer,
    UINT32 decodeBufferStride,
    GRAPH_COLORSPACE decodeColorFormat,
    PGRAPH_RECT pContextRect,
    PGRAPH_RECT pDirtyRects,
    UINT8 dirtyRectCount);

VPSTATUS _VPAPI CleanupH264(VOID);

VPSTATUS _VPAPI CleanupH264Context(PGRAPH_RECT);

VPSTATUS _VPAPI Bitmap_createFromEncoded(PBITMAP_ENCODED_DATA pEncodedData, PBITMAP_DECODER_CALLBACK pRenderCallback, PBITMAP_DECODE_DATA pDecodedData);

VPSTATUS _VPAPI Bitmap_queryEncodedInformation(PBITMAP_ENCODED_DATA pEncodedData, PUINT pDesiredBitsPerPixel, PUINT32 pDecompressedByteCount);

VPSTATUS
_VPAPI
Cursor_create(
    IN   INT32 xHotSpot,
    IN   INT32 yHotSpot,
    IN   PVOID pFaceData,
    IN   PGRAPH_BITMAPINFO pFaceInfo,
    IN   PVOID pMaskData,
    IN   PGRAPH_BITMAPINFO pMaskInfo,
    IN   BOOLEAN fMonochrome,
    IN   HGDC hgDC,
    OUT  PHGCURSOR phCursor );

VPSTATUS
_VPAPI
Cursor_createScaled(
    IN   INT32 xHotSpot,
    IN   INT32 yHotSpot,
    IN   PVOID pFaceData,
    IN   PGRAPH_BITMAPINFO pFaceInfo,
    IN   PVOID pMaskData,
    IN   PGRAPH_BITMAPINFO pMaskInfo,
    IN   BOOLEAN fMonochrome,
    IN   HGDC hgDC,
    OUT  PHGCURSOR phCursor,
    IN   UINT32 scaleFactor,
	IN	 BOOLEAN isRescalable);

VPSTATUS
_VPAPI
Cursor_destroy(
    IN   PHGCURSOR phCursor );

VPSTATUS
_VPAPI
Region_create(
    IN   UINT16 numRects,
    IN   PGRAPH_RECT vRects,
    OUT  PHGREGION phRegion );

VPSTATUS
_VPAPI
Region_destroy(
    INOUT  PHGREGION phRegion );

VPSTATUS
_VPAPI
Region_combine(
    IN   HGREGION hRegion1,
    IN   HGREGION hRegion2,
    IN   GRAPH_REGION_OP op,
    OUT  HGREGION hResult);

VPSTATUS
_VPAPI
Region_getBox(
    IN   HGREGION hRegion,
    OUT  PGRAPH_RECT pRect );

VPSTATUS
_VPAPI
Region_offset(
    IN   HGREGION hRegion,
    IN   INT32    xOffset,
    IN   INT32    yOffset);

VPSTATUS
_VPAPI
Region_startEnum(
    IN   HGREGION hRegion,
    OUT  PPVOID   ppState);

BOOL
_VPAPI
Region_enum(
    INOUT PVOID   pState,
    OUT  PGRAPH_RECT pRect);

VPSTATUS
_VPAPI
Region_endEnum(
    INOUT PVOID   pState);

VPSTATUS
_VPAPI GDC_setUserData( IN HGDC   hGDC,
                        IN PVOID pUserData );

VPSTATUS
_VPAPI GDC_getUserData( IN  HGDC   hGDC,
                        OUT PPVOID ppUserData );

VPSTATUS
_VPAPI
Graph_setCallbacks( IN GRAPH_CALLBACKS *pGraphCallbacks);

VPSTATUS
_VPAPI
Graph_getFontSmoothing( OUT PUSHORT pType );




#endif /* USE_GRAPH_CALL_TABLE */

#if defined(GA_BMP_CACHE) || defined(GA_GLYPH_CACHE)
/*In order to limit number of pixmaps in Xserver , a link list
 *of pixmap Id'd is stored in client.
 *  algorithm for adding and deleting pixmap's
 * Note :- Bitmaps are stored only on second use
 * Algo is as follows:-
 * -If A New image object
     -- if(bitmap) 
           --if first time used
              --create Node and return.
           --else
                -- Calculate its memory requirment
                   mem_required = width * height * (bits_per_pixel/8)
                -- if mem_required>(Max_LIMIT-Mem_already_allocated)
                       Delete from the end of the list till we get
                       sufficient memory
                -- Create Pixmap
                -- Add Pixmap node at the beginning of the list
                -- Update ga_bmp_cache
      -- Else (if glyph)
         -- Create a node
         -- Calculate its memory requirment
                  mem_required = width * height * (bits_per_pixel/8)
         -- if mem_required>(Max_LIMIT-Mem_already_allocated)
                     Delete from the end of the list till we get
                      sufficient memory
         -- Create Pixmap
         -- Add Pixmap node at the beginning of the list
         -- Update ga_bmp_cache 
                
  - Else(already stored)
     -- Get node pointer from ga_bmp_cache table.
     -- Extract information from node to
            draw pixmap to window
     -- Move this node to the head of list.
 *
*/
typedef struct _Pixmap_link_list
{
  GA_BMP_HND          pixmap_id;
  UINT32              mem_acquired; /*memory for this pixmap*/
  int              GACacheHandle;    /*Index into ga_bmp_cache array*/
  
  /*This parameter tells wheather we can delete this node now.
    e.g suppose server has sent TextOut request to draw some glyphs.
    Some glyphs are already in Ga cache but some are not.Now assume that
    there is no room for new pixmap node and we need to delete some node 
    to get space.Deleteion algorithm deletes nodes from tail
    .Now it is possible that the glyphs which we want to draw are
    currently at tail position.Therefore to avoid deleting these nodes
    this parameter has been introduced*/
  BOOL                canDelete;        
  
  struct _Pixmap_link_list *pNextNode;
  struct _Pixmap_link_list *pPrevNode;
}Pixmap_list_node;

typedef struct _Pixmap_list_info_tag
{
    Pixmap_list_node * pHead;
    Pixmap_list_node * pTail;
    INT32              mem_available;       /*Note:It is signed*/
}_Pixmap_list_info_st; 

typedef enum  {DELETE_TAIL_NODE,DELETE_ANY}en_delete;

Pixmap_list_node  *MallocPixmapNode(Pixmap_list_node **ppGaCache,
                                    int gaCacheHandle); 
Pixmap_list_node  *CreatePixmapNode(  /* Display display;*/
                                      _Pixmap_list_info_st *pPixListInfo,
                                      Pixmap_list_node **ppGaCache,
                                      UINT32 width,
                                      UINT32 height,
                                      UINT32 depth,
                                      int gaCacheHandle,
                                      Pixmap_list_node  **pNewNode/*INOUT*/ );
void AddPixmapNodeIntoLinkList(Pixmap_list_node *pPixmapNode,
                                 _Pixmap_list_info_st *pPixListInfo);
UINT32 DeleteFromPixmapList(/*Display display,*/
                          _Pixmap_list_info_st *pPixListInfo,
                          Pixmap_list_node *pNode,
                          Pixmap_list_node **ppGaCache,
                          en_delete del
                          );
void MoveNodeToTheBegining(Pixmap_list_node *pNode,_Pixmap_list_info_st *pPixListInfo); 
#endif

#ifdef GA_BMP_CACHE
/* The low-level cache is invisible to SRCC, so not in the call table. */

/* Create a cached bitmap. */

UINT32 GA_cache_bmp(HGDC dest, UINT8 *data, GRAPH_BITMAPINFO *pbmi,
                               _Pixmap_list_info_st *pPixListInfo,
                               Pixmap_list_node **ppGaCache,
                               int ga_cache_index,
                               Pixmap_list_node* pPixmapNode);
/* Return a GDC handle that can be used to BLT from a cached bitmap
 * to another GDC.
 */

HGDC GA_cache_src(Pixmap_list_node *pNode,_Pixmap_list_info_st *pPixListInfo);

/* Destroy a cached bitmap, and clear the reference. */

void GA_clear_bmp(Pixmap_list_node *pNode,
                  _Pixmap_list_info_st *pPixListInfo,
                  Pixmap_list_node **ppGACache);
#endif /* GA_BMP_CACHE */

#ifdef GA_GLYPH_CACHE

/*This structure stores info for each glyph to be drawn*/
typedef struct{
	Pixmap_list_node *pPixmap;         /*pixmap id */
	UINT32 dest_x;
	UINT32 dest_y;
	UINT32 width;              /*width of glyph*/
	UINT32 height;              /* height of glyph*/

}GA_GLYPH_DATA, *PGA_GLYPH_DATA;

/*This structure contains information about all Glyphs
 * which needs to be drawn (in TextOut Request).*/
typedef struct{
	HGDC 		dest_gdc;

        /*Dimemsions of overall rectangel*/
        UINT32 		rect_x;   
        UINT32 		rect_y;
        UINT32 		rect_width;
        UINT32 		rect_height;
        
        /*Mode. Opaque or Transparent*/
        UINT8     	textMode;

        /* Info of Glyphs to be drawn*/
        UINT32          nGlyphs;  /*How many*/   
	PGA_GLYPH_DATA  pGa_glyphs;  /*Info of each 
                                     *glyph in a request*/

}GA_GLYPH_LIST,*PGA_GLYPH_LIST;

/*Funtions for implementing GA cache for glyphs*/
void GA_CreateGCForGlyph();

void GA_DestroyGlyphGC();

UINT32 GA_DrawRect(HGDC hGdc, UINT32 x, UINT32 y, UINT32 width,
                       UINT32 height,UINT32 fg_pixel,UINT32 bg_pixel,
                       GRAPH_ROP3 rop3);

UINT32 GA_DrawGlyphsFromGaCache( PGA_GLYPH_LIST pGlyph_Info, 
                                 _Pixmap_list_info_st *pPixListInfo);

UINT32 GA_StoreGlyphInCache(HGDC hGdc, 
                            _Pixmap_list_info_st *pPixListInfo,
                            Pixmap_list_node **pGaCache,
                            UINT8 *img_data,UINT32 width,
                            UINT32 height ,
                            int ga_cache_index,
                            Pixmap_list_node **ppPixmapNode);

#endif /*GA_GLYPH_CACHE*/

#endif /* _CTXGRAPH_H_ */

