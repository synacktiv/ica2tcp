/*++ BUILD Version: 0000 ******************************************************
*
*  ICA.H
*     This file contains definitions for the ICA 3.0 structures
*
*     NOTE: this file is shared between the client and the server
*
*  Copyright 2002-2015 Citrix Systems, Inc. 
*
*  This is an unpublished work protected as such under copyright law,
*  containing proprietary and confidential information of Citrix Systems, Inc.
*  Use, disclosure or reproduction without the express written authorization
*  of Citrix Systems, Inc. is prohibited.
*
*******************************************************************************/

#ifndef __ICA_H__
#define __ICA_H__

/*
 * these are done this way (not #if WINFRAME_BUILD) to check to see
 * if we are building on the server side apposed to the client side
 */
#ifdef WINFRAME_BUILD
#ifdef MFIMA
#include <ica30.h>
#else
#include <citrix\ica30.h>
#endif
#endif

/*
 * these are done this way (not #if WINFRAME_BUILD) to check to see
 * if we are building on the server side apposed to the client side
 */
#if defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
#ifdef MFIMA
#include <vrtlclss.h>
#else
#include <citrix\vrtlclss.h>
#endif
#endif

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_ENABLE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
#pragma pack(1)
#endif

/* // begin_icasdk */

/*=============================================================================
==   Client Modules
=============================================================================*/

/*
 *  modules classes
 */
typedef enum _MODULECLASS {
    Module_UserInterface,
    Module_UserInterfaceExt, /* Obsolete */
    Module_WinstationDriver,
    Module_VirtualDriver,
    Module_ProtocolDriver,
    Module_TransportDriver,
    Module_NameResolver, /* Obsolete */
    Module_NameEnumerator, /* Obsolete */
    Module_Scripting, /* Obsolete */
    Module_SubDriver, /* Obsolete */
    ModuleClass_Maximum
} MODULECLASS;
/* // end_icasdk */


/*=============================================================================
==   Protocol Drivers - Common data structures
=============================================================================*/

/*
 *  protocol driver classes
 *
 *  NOTE: don't change the order of this structure it will break
 *  NOTE: the Host.  Also, any additions to this structure must
 *  NOTE: be reflected into the SDCLASS in HYDRIX.H or else we're SOL.
 */
typedef enum _PDCLASS {
    PdNone,            /* 0 */
    PdConsole,         /* 1  !Obsolete! no dll */
    PdNetwork,         /* 2  !Obsolete! tdnetb.dll, tdspx.dll, tdftp.dll tdipx.dll */
    PdAsync,           /* 3  !Obsolete! tdasync.dll */
    PdOemTransport,    /* 4  !Obsolete! user transport driver */
    PdISDN,            /* 5  !Obsolete! not implemented */
    PdX25,             /* 6  !Obsolete! not implemented */
    PdModem,           /* 7  !Obsolete! pdmodem.dll */
    PdOemConnect,      /* 8  !Obsolete! user protocol driver */
    PdFrame,           /* 9  PdRFram.dll. Now used for framing reliable transports only. */
    PdReliable,        /* 10 !Obsolete! pdreli.dll */
    PdEncrypt,         /* 11 pdc0.dll, pdcrypt.dll, pdc40.dll, pdc56.dll, pdc128.dll */
    PdCompress,        /* 12 pdcomp.dll */
    PdTelnet,          /* 13 !Obsolete! not implemented */
    PdOemFilter,       /* 14 !Obsolete! user protocol driver */
    PdNasi,            /* 15 !Obsolete! tdnasi.dll */
    PdTapi,            /* 16 !Obsolete! pdtapi.dll */
    PdReserved1,       /* 17 !Obsolete! */
    PdReserved2,       /* 18 !Obsolete! */
    PdReserved3,       /* 19 !Obsolete! */
    PdClass_Maximum    /* 20 must be last */
} PDCLASS;
/* // begin_icasdk */

/*=============================================================================
==   Virtual Drivers - Common data structures
=============================================================================*/

/*
 *  Virtual Channel Name
 */
#define VIRTUALNAME_LENGTH  7

typedef CHAR VIRTUALNAME[ VIRTUALNAME_LENGTH + 1 ];  /* includes null */
typedef CHAR * PVIRTUALNAME;

/* // end_icasdk */
#define VIRTUAL_MAXIMUM         64    /* number of virtual channels */
/* // begin_icasdk */

/*
 *  Types of virtual i/o flow control
 */
typedef enum _VIRTUALFLOWCLASS {
    VirtualFlow_None,
    VirtualFlow_Ack,
    VirtualFlow_Delay,
    VirtualFlow_Cdm
} VIRTUALFLOWCLASS;

/*
 *  Virtual driver flow control - ack
 */
typedef struct _VDFLOWACK {
    USHORT MaxWindowSize;           /* maximum # of bytes we can write */
    USHORT WindowSize;              /* # of bytes we can write before blocking */
} VDFLOWACK, * PVDFLOWACK;

/*
 *  Virtual driver flow control - delay
 */
typedef struct _VDFLOWDELAY {
    ULONG DelayTime;                /* delay time in 1/1000 seconds */
} VDFLOWDELAY, * PVDFLOWDELAY;

/*
 *  Virtual driver flow control - cdm (client drive mapping)
 */
typedef struct _VDFLOWCDM {
    USHORT MaxWindowSize;           /* total # of bytes we can write */
    USHORT MaxByteCount;            /* maximum size of any one write */
} VDFLOWCDM, * PVDFLOWCDM;

/*
 *  Virtual driver flow control structure
 */
typedef struct _VDFLOW {
    BYTE BandwidthQuota;            /* percentage of total bandwidth (0-100) */
    BYTE Flow;                      /* type of flow control */
    BYTE Pad1[2];
    union _VDFLOWU {
        VDFLOWACK Ack;
        VDFLOWDELAY Delay;
        VDFLOWCDM Cdm;
    } VDFLOWU ;
} VDFLOW, * PVDFLOW;
/* // end_icasdk */

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_RESTORE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
#pragma pack()
#endif

#endif /* __ICA_H__ */
