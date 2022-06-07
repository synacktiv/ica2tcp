/**********************************************************************
*
*  WDAPI.H
*
*  This module contains external winstation driver API
*  defines and structures
*
*  $Id$
*
*  Copyright 1994-2016 Citrix Systems, Inc. All Rights Reserved.
*
***********************************************************************/

#ifndef __WDAPI_H__
#define __WDAPI_H__

#include "dllapi.h"
#include "vrtlclss.h"
#include "engtypes.h"
#include "ica.h"

/*
 *  Index into WD procedure array
 */

#define WDxQUERYINFORMATION      6
#define WDxSETINFORMATION        7
#define WDxCOUNT                 8  /* number of external wd procedures */

/*
 *  WdInformationClass enum
 * 
 *  NOTE: Do not remove or comment out any of
 *  these entries as doing so will probably
 *  break the VCSDK which has its own wdapi.h and
 *  relies on WdOpenVirtualChannel to get a 
 *  virtual channel number.
 */
typedef enum _WDINFOCLASS {
    WdClientData,
    WdStatistics,
    WdLastError,    /* ***** UNUSED ****/
    WdConnect,
    WdDisconnect,
    WdKillFocus,
    WdSetFocus,
    WdEnablePassThrough,
    WdDisablePassThrough,
    WdVdAddress,
    WdVirtualWriteHook,
    WdAddReadHook,
    WdRemoveReadHook,
    WdAddWriteHook,
    WdRemoveWriteHook,
    WdModemStatus,
    WdXferBufferSize,     /* Get: Size of host and client buffers */
    WdCharCode,
    WdScanCode,
    WdMouseInfo,
    WdInitWindow,
    WdDestroyWindow,
    WdRedraw,             /* Tell the host to redraw */
    WdThinwireStack,      /* Pass the thinwire stack to the thinwire vd */
    WdHostVersion,        /* get - Host Version information*/
    WdRealizePaletteFG,   /* inform client to realize it's foreground palette */
    WdRealizePaletteBG,   /* inform client to realize it's background palette */
    WdInactivate,         /* client is about to lose input focus */
    WdSetProductID,
    WdGetTerminateInfo,   /* test for legitimate terminate */
    WdRaiseSoftkey,       /* raise the soft keyboard */
    WdLowerSoftkey,       /* lower the soft keyboard */
    WdIOStatus,           /* IO status    ***UNUSED *** */
    WdOpenVirtualChannel, /* get - Open a virtual channel */
    WdCache,              /* persistent caching command set */
    WdGetInfoData,        /* Dead: Information from host to client */
    WdWindowSwitch,       /* window has switched back, check keyboard state */
#if defined(UNICODESUPPORT) || defined(USE_EUKS)
    WdUnicodeCode,        /* window has sent unicode information to wd */
#endif
#ifdef PACKET_KEYSYM_SUPPORT
    WdKeysymCode,         /* window has sent keysym information to wd */
#endif
#ifdef WIN32
    WdSetNetworkEvent,    /* This is for engine to pass a event handle down to TD if WinSock2 is suported. */
#endif
    WdPassThruLogoff,
    WdClientIdInfo,              /* Get the client identification information */
    WdPartialDisconnect,         /* A PN Refresh connection will disconnect */
    WdDesktopInfo,               /* Get/Set: information about the remote dekstop */
    WdSeamlessHostCommand,       /* Set: Seamless Host Agent command */
    WdSeamlessQueryInformation,  /* Get: Seamless information */
#if defined(UNICODESUPPORT) || defined(USE_EUKS)
    WdZlRegisterUnicodeHook,     /* Set: Zero Latency Unicode hook registration*/
    WdZLUnRegisterUnicodeHook,   /* Set: Zero Latency Unicode hook unregistration*/
#endif
    WdZLRegisterScanCodeHook,    /* Set: Zero Latency scan code hook registration*/
    WdZlUnregisterScanCodeHook,  /* Set: Zero Latency scan code hook unregistration*/
    WdIcmQueryStatus,            /* Get: Ica Channel Monitoring status */
    WdIcmSendPingRequest,        /* Set: Send ping request and call callback if specified */
    WdSetCursor,                 /* Set: New cursor from TW. Data - PHGCURSOR */
    WdFullScreenMode,            /* Get: Return TRUE if full screen text is enabled */
    WdFullScreenPaint,           /* Set: Full Screen Mode needs redrawn*/
    WdSeamlessInfo,              /* Get: Seamless information/capabilities from WD */
    WdCodePage,                  /* Get: Retrieve server/client-default codepage */
    WdIcaControlCommand,         /* Set: ICA Control Command */
    WdReconnectInfo,             /* Get: Information needed for auto reconnect */
    WdServerSupportBWControl4Printing, /* return TRUE if server suports printer bandwidth control*/
    WdVirtualChannel,            /* Get: Virtual channel information */
    WdGetLatencyInformation,     /* Get: Latency information */
    WdKeyboardInput,             /* Get/Set: Enable/Disable keyboard input */
    WdMouseInput,                /* Get/Set: Enable/Disable mouse input */
    WdCredentialPassing,         /* Set: Passing Credentials through WD */
    WdRenderingMode,             /* Set: Virtual channel the rendering mode (Headless) */
    WdPauseResume,               /* Get/Set: Pause/Resume virtual channels */
    WdQueryMMWindowInfo,         /* Get: Query the information for the MMVD */
    WdGetICAWindowInfo,          /* Get: Get info about the ICA window */
    WdICOSeamlessFunctions,      /* Get/Set: ICO command */
#ifdef USE_EUKS
    WdEUKSVersion,               /* Get: Get Server EUKS version. */
#endif
    WdSetC2HPriority,            /* Unused */
    WdPnP,                       /* Set: Send CLPNP_COMMAND commands inbetween the control VC and the */
                                 /*      implmentation VC (such as VIRTUAL_CCM) */
    /* Support for EUEM (Darwin Release For Ohio). This code has been added or
     * changed to support End User Experience Monitoring functionality. Please do
     * not change the code in this section without consulting the EUEM team
     *  Email (at the time of change) "#ENG - Darwin Dev".
     */
    WdEuemEndSLCD,               /* Set endSLCD */
    WdEuemStartupTimes,          /* Get/Set. Set startup times from wfcrun32. Get startup times from VD EUEM  */
    WdEuemTwCallback,            /* Set: register the EUEM ICA roundtrip callback */
                                 /*      function from the thinwire VC to the EUEM VC */
    WdSessionIsReconnected,      /* Get. Get indication if the session was auto reconnected using a token from VD EUEM  */
    WdUserActivity               /* Get/Set. Get indication if there has been user activity in the session from VD EUEM  */
#ifdef WINCE
    ,WdEuemApplicationName       /* The name of the application used by EUEM */ 
#endif
    /* End of EUEM support section */
    ,WdLicensedFeatures           /* Get: Get Licensed Features */
    ,WdResizeHotBitmapCache       /* Set: Tell thinwire driver to resize the hot bitmap cache */
    ,WdLockDisplay                  /* The Server has had its screen locked */
#ifdef UDP_AUDIO
    ,WdRtpSetupInformation      /* VD->WD info about UDP port range, initialize RTP structures */
    ,WdRtpInitClientHandshake   /* VD->WD - start waiting for server->client handshake */
    ,WdRtpSetup                 /* VD->WD - send ACK on TCP to server (final handshake) */
#endif
    ,WdQueryVCNumbersForVD              /* Get: a list of virtual channels numbers that were registered since 
                                        last call to WdQueryVCNumbersForVD*/
    ,WDCheckOutTicket                // Check out ticket to authenticate to UDP Gateway
	,WDCheckInTicket                     // Check in new ticket received from UDP Gateway
} WDINFOCLASS;

/*
 *  WdQueryInformation structure
 */
typedef struct _WDQUERYINFORMATION {
    WDINFOCLASS WdInformationClass;
    LPVOID pWdInformation;
    USHORT WdInformationLength;
    USHORT WdReturnLength;
} WDQUERYINFORMATION, * PWDQUERYINFORMATION;

/*
 *  WdSetInformation structure
 */
typedef struct _WDSETINFORMATION {
    WDINFOCLASS WdInformationClass;
    LPVOID pWdInformation;
    USHORT WdInformationLength;
} WDSETINFORMATION, * PWDSETINFORMATION;



/*
 *  WdOpenVirtualChannel structure
 */
typedef struct _OPENVIRTUALCHANNEL {
    LPVOID  pVCName;
    USHORT  Channel;
} OPENVIRTUALCHANNEL, * POPENVIRTUALCHANNEL;

/*
 * WdQueryVCNumbersForVD structure
 */
typedef struct _QUERYVCNUMBERSFORVD {
    USHORT VcCountForVD;
    USHORT VcNumbersForVD[VIRTUAL_MAXIMUM];
} QUERYVCNUMBERSFORVD, *PQUERYVCNUMBERSFORVD;


#define INVALID_VC_HANDLE 0xffff

/*
 *  WdMouseInfo structure
 */
typedef struct _MOUSEINFO {
    USHORT  cMouseData;
    LPVOID  pMouseData;
} MOUSEINFO, * PMOUSEINFO;


/*
 *  Set Info Class enum
 */
typedef enum _SETINFOCLASS {
    set_info_unused
} SETINFOCLASS, * PSETINFOCLASS;

/*
 *  Query Info Class enum
 */
typedef enum _QUERYINFOCLASS {
    QueryHostVersion,
    OpenVirtualChannel
} QUERYINFOCLASS, * PQUERYINFOCLASS;

/*
 *  Outbuf Buffer data structure
 */
typedef struct _OUTBUF {
    struct _OUTBUF * pLink;      /* wd/td outbuf linked list */
    LPBYTE pMemory;              /* pointer to allocated buffer memory */
    LPBYTE pBuffer;              /* pointer within buffer memory */
    USHORT MaxByteCount;         /* maximum buffer byte count (static) */
    USHORT ByteCount;            /* byte count pointed to by pBuffer */
} OUTBUF, * POUTBUF;

/*
 * WdIcmSendPingRequest structure
 */
typedef struct _WDICMSENDPINGREQUEST
{
   PVOID    pfnCallback;        /* Callback function. Can be NULL */
   PVOID    Context;            /* Callback context. Can be NULL  */
   BOOL     fUrgent;            /* if TRUE then flush on the next poll */

} WDICMSENDPINGREQUEST, *PWDICMSENDPINGREQUEST;


typedef int  (PWFCAPI POUTBUFALLOCPROC)( LPVOID, POUTBUF * );
typedef void (PWFCAPI POUTBUFFREEPROC)( LPVOID, POUTBUF );
typedef int  (PWFCAPI PPROCESSINPUTPROC)( LPVOID, LPBYTE, USHORT, int );
typedef int  (PWFCAPI PSETINFOPROC)( LPVOID, SETINFOCLASS, LPBYTE, USHORT );
typedef void (PWFCAPI PIOHOOKPROC)( LPBYTE, USHORT );


typedef int  (PWFCAPI PQUERYINFOPROC)( LPVOID, QUERYINFOCLASS, LPBYTE, USHORT );
typedef int  (PWFCAPI POUTBUFRESERVEPROC)( LPVOID, USHORT );
typedef int  (PWFCAPI POUTBUFAPPENDPROC)( LPVOID, LPBYTE, USHORT );
typedef int  (PWFCAPI POUTBUFWRITEPROC)( LPVOID );
typedef int  (PWFCAPI PAPPENDVDHEADERPROC)( LPVOID, USHORT, USHORT );

#define FLUSH_IMMEDIATELY                   0

typedef struct _MEMORY_SECTION {
    UINT    length;
    LPBYTE  pSection;
} MEMORY_SECTION, far * LPMEMORY_SECTION;

typedef int (PWFCAPI PQUEUEVIRTUALWRITEPROC) (LPVOID, USHORT, LPMEMORY_SECTION, USHORT, USHORT);

/*=============================================================================
 ==   Virtual driver hook structures
 ============================================================================*/

/*
 *  Virtual driver write hook function.
 */
typedef int (*PVDWRITEPROCEDURE)(LPVOID, USHORT, LPBYTE, USHORT);

/*
 *  Used when registering virtual write hook  (WdVirtualWriteHook)
 */
typedef struct _VDWRITEHOOK {
    USHORT Type;                             /* in: virtual channel id */
    LPVOID pVdData;                          /* in: pointer to virtual driver data */
    PVDWRITEPROCEDURE pProc;                 /* in: pointer to vd write procedure (wd->vd) */
    LPVOID pWdData;                          /* out: pointer to wd structure */
    POUTBUFRESERVEPROC pOutBufReserveProc;   /* out: pointer to OutBufReserve */
    POUTBUFAPPENDPROC pOutBufAppendProc;     /* out: pointer to OutBufAppend */
    POUTBUFWRITEPROC pOutBufWriteProc;       /* out: pointer to OutBufWrite */
    PAPPENDVDHEADERPROC pAppendVdHeaderProc; /* out: pointer to AppendVdHeader */
    USHORT MaximumWriteSize;                 /* out: maximum ica write size */
    PQUEUEVIRTUALWRITEPROC pQueueVirtualWriteProc;  /* out: pointer to QueueVirtualWrite */
} VDWRITEHOOK, * PVDWRITEHOOK;



/* The opaque handle to the WD itself. */

typedef struct _WD * PWD;
struct _TRANSPORT_CONNECTION;

/* QueueVirtualWrite() is now the preferred virtual driver write function. */

int WFCAPI QueueVirtualWrite(PWD, USHORT, LPMEMORY_SECTION, USHORT, USHORT);

typedef int (*PICAPROCEDURE)(struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);

int IcaInitRequest      (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaInitConnect      (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaTerminate        (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaStopOk           (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaMsiRequest       (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaBeep             (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaSetMousePosition (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaVirtualWrite     (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaVirtualAck       (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int SetKeyboardState    (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaVirtualFlush     (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaClientData       (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaCMCommand        (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaCookie           (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);
int IcaSendCreds        (struct _TRANSPORT_CONNECTION *, LPBYTE, USHORT);

/* Simplified interfaces to WD. */

struct _IOSTATUS;

int Wd_GetIOStatus(struct _IOSTATUS *IOStatus, int stream_id);
int Wd_GetLastError(PWFELASTERROR pLastError, int stream_id);
int Wd_ConnectStream(int stream_id);
int Wd_Disconnect(void);
int PacketPriorityCall(struct _TRANSPORT_CONNECTION *pConnect, int PacketPriority);
#ifdef AUTORECNX
BOOL Wd_GetTerminateInfo(void);
#endif /* AUTORECNX */

/* Allocate a virtual channel number, or confirm a conditional allocation,
 * passing the data input function for the channel, and its first argument.
 * Returns a channel number or INVALID_VC_HANDLE.  To allocate a previously
 * free channel, use INVALID_VC_HANDLE for preferred_channel.
 */

int WDAttachVC(char *vc_name, int preferred_channel,
               PVDWRITEPROCEDURE func, void *param);

/* Retrieve one a small number of text strings sent by the server. */

int WdGetInfoForId(char *Id, LPSTR pData, USHORT Length, USHORT *ReturnLength);

/* Functions to send user input. */

struct _UNICODECODE;
int WD_send_unicode(struct _UNICODECODE *);

struct WND_MOUSEDATA;
int MouWrite(PWD pWd, struct WND_MOUSEDATA *pMouData, USHORT MouCount);
struct _MOUSEDATARAW;
void Send_raw_mouse_data(PWD, struct _MOUSEDATARAW *);
void Send_raw_mouse_queue(PWD);

/* Automatic session reconection support. */

struct _recnx_info;
void WDSet_Reconnect_Info(struct _recnx_info *);
void WDGet_Reconnect_Info(struct _recnx_info *);
BOOLEAN WDIsSessionReconnected(void);
#endif /* __WDAPI_H__ */
