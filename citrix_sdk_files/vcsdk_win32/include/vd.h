/***************************************************************************
*
*  VD.H
*
*  This module contains Virtual Driver (PD) defines and structures
*
*  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/vd.h#1 $
*
*  Copyright 1998-2015 Citrix Systems, Inc. 
*
****************************************************************************/

#ifndef __VD_H__
#define __VD_H__

#pragma once

#include <dllapi.h>

/*=============================================================================
==   typedefs
=============================================================================*/

typedef struct _VD * PVD;

/*
 *  VD structure
 */
typedef struct _VD {

    /* This field can be either a bit mask of supported channels (b0=0) OR 
     * it can be the actual number of a supported channel. 
     * The correct interpretation will be determined by the WD and hence 
     * this field should not be used to set/get any information about the supported channels. */
    ULONG ChannelMask;                  

    PDLLLINK pWdLink;                   /* pointer to winstation driver */
    int LastError;                      /* Last Error code */
    PVOID pPrivate;                     /* pointer to VD uncommon data */

} VD;

/*=============================================================================
==   function prototypes
=============================================================================*/
extern int VdCallWd(PVD pVd, USHORT ProcIndex, PVOID pParam, PUINT16 puiSize);

#ifdef SIMPLIFIED_LINKAGE
/* The declaration of VdLibMgr() as extern causes an error with gcc 4.x
 * compiler, as the implementation is declared as STATIC.
 */
#define VdLibMgr() g_pLibMgrCallTable
#else
extern PLIBMGRTABLE VdLibMgr();
#endif

#endif /* __VD_H__ */
