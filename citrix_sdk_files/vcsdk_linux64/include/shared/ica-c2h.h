/*++ BUILD Version: 0000 ******************************************************
*
*  ICA-C2H.H   client -> host
*
*     This file contains structures that are sent by the client to
*     the host at connection time.
*
*  Copyright 1998-2015 Citrix Systems, Inc. 
*
*  This is an unpublished work protected as such under copyright law,
*  containing proprietary and confidential information of Citrix Systems, Inc.
*  Use, disclosure or reproduction without the express written authorization
*  of Citrix Systems, Inc. is prohibited.
*
*******************************************************************************/

#ifndef __ICAC2H_H__
#define __ICAC2H_H__

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_ENABLE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD) || (defined(USE_PACK_PRAGMA) && defined(linux)) 
#pragma pack(1)
#endif
/* // begin_icasdk */

/*=============================================================================
==   Common Module Header
=============================================================================*/

/*
 *  Common module header (36 bytes)
 */
typedef struct _MODULE_C2H {
    USHORT ByteCount;               /* length of module data in bytes (<2k) */
    BYTE ModuleCount;               /* number of modules headers left to be sent */
    BYTE ModuleClass;               /* module class (MODULECLASS) */
    BYTE VersionL;                  /* lowest supported version */
    BYTE VersionH;                  /* highest supported version */
    BYTE ModuleName[13];            /* client module name (8.3) */
    BYTE HostModuleName[9];         /* optional host module name (9 characters) */
    USHORT ModuleDate;              /* module date in dos format */
    USHORT ModuleTime;              /* module time in dos format */
    ULONG ModuleSize;               /* module file size in bytes */
} MODULE_C2H, * PMODULE_C2H;
/* // end_icasdk */
/* // begin_icasdk */

/*
 *  common virtual driver header
 */
typedef struct _VD_C2H {
    MODULE_C2H Header;

    /* This field is interpreted as follows:
     * 
     * If the CAPABILITY_VIRTUAL_MAXIMUM is not supported by both the client and the host then:
     *   -  The ChannelMask field is interpreted as a bit mask of Virtual Channels (VCs) 
     *      supported by a client Virtual Driver (VD), where b0=0.
     *   -  Always a single VD_C2H structure is sent per client VD.
     *
     * Otherwise, if the CAPABILITY_VIRTUAL_MAXIMUM is supported by both the client and the host then:
     *   -  The ChannelMask field is interpreted as a single VC number.
     *   -  If a single client VD module handles more than one VC, then for this VD 
     *      multiple VD_C2H structures are sent - one for each VC that the VD supports. 
     *      These modules are sent in separate PACKET_INIT_RESPONSE packets and are 
     *      identical to each other except for the Header.ModuleCount and ChannelMask fields.
     */
    ULONG ChannelMask; 

    VDFLOW Flow;
} VD_C2H, * PVD_C2H;
/* // end_icasdk */

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_RESTORE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD) || (defined(USE_PACK_PRAGMA) && defined(linux))
#pragma pack()
#endif

#endif /* __ICAC2H_H__ */
