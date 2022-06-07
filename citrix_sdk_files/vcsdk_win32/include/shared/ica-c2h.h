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
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
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

#define VERSION_CLIENTL_TDTCPFTP  1
#define VERSION_CLIENTH_TDTCPFTP  2

#define VERSION_CLIENTL_TDTCPNOV  1
#define VERSION_CLIENTH_TDTCPNOV  2

#define VERSION_CLIENTL_TDTCPMS   1
#define VERSION_CLIENTH_TDTCPMS   2

#define VERSION_CLIENTL_TDTCPVSL  1
#define VERSION_CLIENTH_TDTCPVSL  2

/*
 *  transport drivers (tdnetb.dll, tdasync.dll, tdipx.dll, .... )
 */
typedef struct _TD_C2H {

    /* version 1 */
    MODULE_C2H Header;
    USHORT OutBufLength;        /* length of outbufs in bytes */

    /* version 2 */
    USHORT ClientAddressFamily; /* address family of ClientAddress */
    BYTE ClientAddress[20];     /* client network address */
} TD_C2H, * PTD_C2H;

#ifndef MacOrUnix		/* In socket.h on these systems. */
/*
 * Address families.
 */
#define AF_UNSPEC       0               /* unspecified */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#undef AF_IPX   // there are different defines for this in winsock.h and winsock2.h
#define AF_IPX          6               /* IPX and SPX */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_INET6        23              /* Internetwork Version 6 */
#endif /* MacOrUnix */

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_RESTORE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
#pragma pack()
#endif

#endif /* __ICAC2H_H__ */
