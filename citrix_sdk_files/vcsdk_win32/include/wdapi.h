/**********************************************************************
*
*  Copyright (C) Citrix Systems, Inc. All Rights Reserved.
*
*  WDAPI.H
*
*  This module contains external winstation driver API
*  defines and structures
*
*  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/wdapi.h#1 $
*
***********************************************************************/

#ifndef __WDAPI_H__
#define __WDAPI_H__

#pragma once

#if defined(PLATFORM_PRAGMAS) && defined(MKDBG)
#if defined(__DLLAPI_H__) && defined(_MINIDLL_H_)
#pragma MKDBG("DLLAPI and MINIDLL are included")
#elif defined(_MINIDLL_H_)
#pragma MKDBG("MINIDLL is included")
#elif defined(__DLLAPI_H__)
#pragma MKDBG("DLLAPI is included")
#endif
#endif /* PLATFORM_PRAGMAS && MKDBG */

#include <dllapi.h>
#include <vrtlclss.h>
#include <engtypes.h>
#include <ica.h>
#include <Api_HpcCallTable.h>

/*
 *  Index into WD procedure array
 */

#define WDxQUERYINFORMATION      6
#define WDxSETINFORMATION        7
#define WDxCOUNT                 8  /* number of external wd procedures */

/*
 *  WdInformationClass enum
 * 
 *  IMPORTANT NOTE:   DO NOT REORDER OR DELETE ENTRIES FROM THIS ENUM STRUCTURE.   
 *                    ALWAYS ADD TO THE END OF THE LIST.  
 *                    Customers use this header file and we want to maintain forward compatibility.
 */
typedef enum _WDINFOCLASS {
    WdClientData,
#ifndef unix
    WdStatistics,
#endif /* unix */
    WdLastError,
    WdConnect,
    WdDisconnect,
    WdKillFocus,
    WdSetFocus,
#ifndef unix
    WdEnablePassThrough,
    WdDisablePassThrough,
#endif /* unix */
    WdVdAddress,
    WdVirtualWriteHook,
    WdAddReadHook,
    WdRemoveReadHook,
#ifndef unix
    WdAddWriteHook,
    WdRemoveWriteHook,
    WdModemStatus,
#endif /* unix */
    WdXferBufferSize,     /* Get: Size of host and client buffers */
#ifndef unix
    WdCharCode,
#endif /* unix */
    WdScanCode,
    WdMouseInfo,
    WdInitWindow,
    WdDestroyWindow,
    WdRedraw,             /* Tell the host to redraw */
#ifndef unix
    WdThinwireStack,      /* Pass the thinwire stack to the thinwire vd */
#endif /* unix */
    WdHostVersion,        /* get - Host Version information*/
#ifndef unix
    WdRealizePaletteFG,   /* inform client to realize it's foreground palette */
    WdRealizePaletteBG,   /* inform client to realize it's background palette */
    WdInactivate,         /* client is about to lose input focus */
#endif /* unix */
    WdSetProductID,
    WdGetTerminateInfo,   /* test for legitimate terminate */
    WdRaiseSoftkey,       /* raise the soft keyboard */
    WdLowerSoftkey,       /* lower the soft keyboard */
    WdIOStatus,           /* IO status */
    WdOpenVirtualChannel, /* get - Open a virtual channel */
    WdCache,              /* persistent caching command set */
    WdGetInfoData,        /* Information from host to client */
#ifndef unix
    WdWindowSwitch,       /* window has switched back, check keyboard state */
#endif /* unix */
    WdUnicodeCode,        /* window has sent unicode information to wd */
#ifdef PACKET_KEYSYM_SUPPORT
    WdKeysymCode,         /* window has sent keysym information to wd */
#endif
#ifdef WIN32
    WdSetNetworkEvent,    /* This is for engine to pass a event handle down to TD if WinSock2 is suported. */
#endif
    WdPassThruLogoff,
    WdClientIdInfo,              /* Get the client identification information */
#ifndef unix
    WdPartialDisconnect,         /* A PN Refresh connection will disconnect */
#endif /* unix */
    WdDesktopInfo,               /* Get/Set: information about the remote dekstop */
    WdSeamlessHostCommand,       /* Set: Seamless Host Agent command */
    WdSeamlessQueryInformation,  /* Get: Seamless information */
    WdZlRegisterUnicodeHook,     /* Set: Zero Latency Unicode hook registration*/
    WdZLUnRegisterUnicodeHook,   /* Set: Zero Latency Unicode hook unregistration*/
    WdZLRegisterScanCodeHook,    /* Set: Zero Latency scan code hook registration*/
    WdZlUnregisterScanCodeHook,  /* Set: Zero Latency scan code hook unregistration*/
    WdIcmQueryStatus,            /* Get: Ica Channel Monitoring status */
#ifndef unix
    WdIcmSendPingRequest,        /* Set: Send ping request and call callback if specified */
#endif /* unix */
    WdSetCursor,                 /* Set: New cursor from TW. Data - PHGCURSOR */
    WdFullScreenMode,            /* Get: Return TRUE if full screen text is enabled */
    WdFullScreenPaint,           /* Set: Full Screen Mode needs redrawn*/
    WdSeamlessInfo,              /* Get: Seamless information/capabilities from WD */
    WdCodePage,                  /* Get: Retrieve server/client-default codepage */
    WdIcaControlCommand,         /* Set: ICA Control Command */
    WdReconnectInfo,             /* Get: Information needed for auto reconnect */
    WdServerSupportBWControl4Printing, /* return TRUE if server suports printer bandwidth control*/
#ifndef unix
    WdVirtualChannel,            /* Get: Virtual channel information */
#endif /* unix */
    WdGetLatencyInformation,     /* Get: Latency information */
#ifndef unix
    WdKeyboardInput,             /* Get/Set: Enable/Disable keyboard input */
    WdMouseInput,                /* Get/Set: Enable/Disable mouse input */
#endif /* unix */
    WdCredentialPassing,         /* Set: Passing Credentials through WD */
#ifndef unix
    WdRenderingMode,             /* Set: Virtual channel the rendering mode (Headless) */
    WdPauseResume,               /* Get/Set: Pause/Resume virtual channels */
#endif /* unix */
    WdQueryMMWindowInfo,         /* Get: Query the information for the MMVD */
#ifndef unix
    WdICOSeamlessFunctions,      /* Get/Set: ICO command */
#endif /* unix */
    WdEUKSVersion,               /* Get: Get Server EUKS version. */
    WdSetC2HPriority,            /* Set: Set the virtual channel pritory from client to server */
                                 /* IMPORTANT: YOU CAN ONLY SET THE PRIOROTY BEFORE YOU HAVE */
                                 /* SENT ANY PROTOCOL FOR THE VC */
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
    ,WdLockDisplay                /* The Server has had its screen locked */
    ,WdCodePageUTF8Support       /* Get: CodePage 65001 (UTF-8) support status */

    ,WdCreateChannelComms         /* Get: Provide the driver with a pipe to communicate directly with. */

#ifdef WIN32
	,WdGetICAWindowInfo					/* Get: Get info about the ICA window */
	,WdGetClientWindowFromServerWindow	/* Get: Get the client window corresponding to the given server window */
	,WdRegisterWindowChangeCallback		/* Get: Register a callback for when the ICA window changes */
	,WdUnregisterWindowChangeCallback	/* Get:	Unregister the ICA window change callback */
#endif // WIN32

    //added for 64 VC support
    ,WdQueryVCNumbersForVD,              /* Get: a list of virtual channels numbers that were registered since 
                                        last call to WdQueryVCNumbersForVD*/
	// last call to WdQueryVCNumbersForVD
	WdVirtualWriteHookEx,				// Added 6/19/2009 by LHR for MTCA compatible channels
	WdInternalFunction,					// Used mostly for debug purposes, params specific to version under test
	WdSendVirtualAck,					// Send virtual ack ICA packet for a given channel
	WdNotifyEuemOfReconnect,			// Send a "set information" to EUEM that a reconnect has occurred (sent by CGP)
#ifdef WIN32
	WdSecondAppStarts,					// Set: send information if second app starts for an RDS server. Helpful in starting USB notifications in case of a Prelaunch session
#endif
	WdRequestPoll,						// VD is requesting to be polled to send data (HPC API Version >= 1)
	WdHpcProperties,					// VD will use the specified version of the HPC API (HPC API Version >= 1)
	WdAddressInfo,						// Get: retrieve resolved address info (used by VDSSPI)
	WdSetDefaultDisplayVc,				// Set: sets/resets the caller/thinwire to be the default display VC 
    WdQueryHostWindowId,                // Get Host Window ID
    WdRtpSetup,                         // Set: mechanism for sending RTP_SETUP commands.
#ifdef WIN32
	WdSeamlessFocusInfo,                // Set: send keyboard focus state of seamless application to Generic USB virtual channel
#endif // WIN32
	WdQueryUdpGatewayAddress,           // Get address of UDP Gateway for tunneling media over UDP.
	WDCheckOutTicket,                   // Check out ticket to authenticate to UDP Gateway
	WDCheckInTicket,                     // Check in new ticket received from UDP Gateway
	WdSendMTCommand,
	WdGetMTInfo,                          // Get multitouch information
	WdSetInputVc,                        // Set: sets the vc for input handling 
	WdSeamlessIMEHotKeySet,
	WdSeamlessSetIMEInformation,
	WdSeamlessQueryIMEInformation,
	WdReceiverAnalytics,
	WdTranslateOverlayWindow

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

// WDSET_HPC_PROPERITES: The VD passes this structure to Wd_Set(WDAPI_USEHPCAPIVERSION) 
// The VD is informing the engine of the HPC VD API version that the VD will use. 
// Possible return codes from the Wd_Set/Get(WDAPI_USEHPCAPIVERSION): 
//    o CLIENT_STATUS_SUCCESS:  OK 
//    o CLIENT_ERROR_INVALID_PARAMETER: Invalid parameter. 
//    o CLIENT_ERROR_INVALID_CHANNEL: The pWdData structure passed is invalid. 
//    o CLIENT_ERROR_VD_NOT_FOUND: The primary thread for the channel was not found. 

typedef struct _WDSET_HPC_PROPERITES 
{ 
	LPVOID    pWdData;            // pointer to the WdData structure provided by the engine. 
	ULONG    ulVdOptions;        // HPC options pertaining to VD 
	USHORT    usVersion;            // the HPC VD API version that will be used. 
} WDSET_HPC_PROPERITES, *PWDSET_HPC_PROPERITES; 

// WDSET_REQUESTPOLL: The VD passes this structure to Wd_Set(WDAPI_REQUESTPOLL)
// The VD is asking to be polled.
// Possible return codes from the Wd_Set(WDAPI_REQUESTPOLL):
//	o CLIENT_STATUS_SUCCESS:  OK
//	o CLIENT_ERROR_QUEUE_FULL: A request to poll is already queued, either from a
//							prior Wd_Set(WDAPI_REQUESTPOLL), or from a call to
//							Hpc_Comm_ApiSendData with the SENDDATA_NOTIFY flag set that
//							returned with CLIENT_ERROR_NO_OUTBUF.
//	o CLIENT_ERROR_INVALID_PARAMETER: Invalid parameter.
//	o CLIENT_ERROR_INVALID_CHANNEL: The pWdData structure passed is invalid.
//	o CLIENT_ERROR_VD_NOT_FOUND: The primary thread for the channel was not found.
//  o CLIENT_ERROR_VD_STILL_BUSY: A previous requested poll has not yet been delivered.
//  o CLIENT_ERROR_VD_POLLING_NOT_READY: The engine is not yet ready to begin polling.

typedef struct _WDSET_REQUESTPOLL
{
	LPVOID	pWdData;			// pointer to the WdData structure provided by the engine.
	LPVOID pUserData;			// a pointer to arbitrary user that will be returned to the
	// VD via DriverPoll(HPCPOLL_FUNCTION_REQUESTEDPOLL)
} WDSET_REQUESTPOLL, *PWDSET_REQUESTPOLL;

/*
 *  WdOpenVirtualChannel structure
 */
typedef struct _OPENVIRTUALCHANNEL {
    LPVOID  pVCName;
    USHORT  Channel;
} OPENVIRTUALCHANNEL, * POPENVIRTUALCHANNEL;

/*
 * **** Added for 64 VC support ****
 * WdQueryVCNumbersForVD structure
 */
typedef struct _QUERYVCNUMBERSFORVD {
    USHORT VcCountForVD;
    USHORT VcNumbersForVD[VIRTUAL_MAXIMUM];
} QUERYVCNUMBERSFORVD, *PQUERYVCNUMBERSFORVD;

#define INVALID_VC_HANDLE 0xffff


/*
 *  Set Info Class enum
 */
typedef enum _SETINFOCLASS {
    CallbackComplete
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

    /*
     *  Non-inherited fields
     */
    struct _OUTBUF * pLink;      /* wd/td outbuf linked list */
    LPVOID pParam;               /* pointer to allocated parameter memory */
    LPBYTE pMemory;              /* pointer to allocated buffer memory */
    LPBYTE pBuffer;              /* pointer within buffer memory */
    USHORT MaxByteCount;         /* maximum buffer byte count (static) */
    USHORT ByteCount;            /* byte count pointed to by pBuffer */

    LPBYTE pSaveBuffer;          /* saved outbuf buffer pointer */
    USHORT SaveByteCount;        /* saved outbuf byte count */

    /*
     *  Inherited fields (when pd allocates new outbuf and copies the data)
     */
    ULONG StartTime;            /* pdreli - transmit time (used to measure roundtrip) */
#ifdef PLATFORM_PRAGMAS
    #pragma warning(disable : 4214)
#endif
    USHORT fControl: 1;         /* pdreli - control buffer (ack/nak) */
    USHORT fRetransmit: 1;      /* pdreli - outbuf retransmit */
    USHORT fCompress: 1;        /* pdcomp - compress data */
#ifdef PLATFORM_PRAGMAS
    #pragma warning(default : 4214)
#endif
    BYTE Sequence;              /* pdreli - sequence number */
    BYTE Fragment;              /* pdreli - fragment number */

} OUTBUF, * POUTBUF;

/*
 * WdIcmQueryStatus structure
 */
typedef struct _WDICMQUERYSTATUSDATA
{
   BOOL    fHostAvailable;  /* Host can support ICM    */
   UINT8   Version;         /* Agreed protocol version */

   BOOL    fPingEnabled;    /* The client can send ping packets      */
   BOOL    fBMEnabled;      /* Background channel monitoring enabled */

   UINT16  uBMRepeatDelay;  /* Background channel monitoring repeat delay, sec. */

   UINT32  LastLatency;
   UINT32  AverageLatency;

} WDICMQUERYSTATUSDATA, *PWDICMQUERYSTATUSDATA;

/*
 * WDSEAMLESSINFO structure
 */
typedef struct _WDSEAMLESSINFO
{
   BOOL     fSeamless20Logon;      /* The server can support seamless 2.0 logon */
   BOOL     fSynchronousSeamless;  /* MF20 synchronous seamless */

   BOOL     fEnabled;              /* Seamless mode enabled for this session */
   BOOL     fPaused;               /* Host agent paused (simulated full screen */

} WDSEAMLESSINFO, *PWDSEAMLESSINFO;

/*
 * WdIcmSendPingRequest structure
 */
typedef struct _WDICMSENDPINGREQUEST
{
   PVOID    pfnCallback;        /* Callback function. Can be NULL */
   PVOID    Context;            /* Callback context. Can be NULL  */
   BOOL     fUrgent;            /* if TRUE then flush on the next poll */

} WDICMSENDPINGREQUEST, *PWDICMSENDPINGREQUEST;

/*
 *  WdReconnectInfo structure. This is now selectively enabled since it was updated to support IPV6.
 *  We have to include winsock2.h for the SOCKADDR_STORAGE type, however this breaks the build for
 *  a number of existing components. By having it disabled by default existing components will build
 *  correctly, while components that need it can enable it as required.
 */
#ifdef USE_IPV6_RECONNECTINFO

#include <winsock2.h>

typedef struct _WDRECONNECTINFO
{
	UINT32       ReconnectManagerId;
	BOOL         bAddressValid; // Name, ResolvedAddress, ClientName, and PortNum are valid
	ADDRESSW     ConnectionFriendlyNameW;
	ADDRESS      Name;
	SOCKADDR_STORAGE ResolvedAddress;
	ADDRESS      IntermediateAddress;
	CLIENTNAMEW  ClientNameW;
	USHORT       PortNum;
	ADDRESS      SSLGatewayName;
	USHORT       SSLGatewayPortNum;
	USHORT       CGPPortNum;
	USHORT       CGPBrPortNum;
	UINT16       cbCookie;	 // Input zero to query for cookie size
	PVOID        cookie[1];  // Placeholder - cookie begins here in the buffer.
	                         // This should always be the last field in this structure.
	                         // since cookies will overwrite any further fields.
} WDRECONNECTINFO, *PWDRECONNECTINFO;

#endif // USE_IPV6_RECONNECTINFO

typedef int  (PWFCAPI POUTBUFALLOCPROC)( LPVOID, POUTBUF * );
typedef void (PWFCAPI POUTBUFFREEPROC)( LPVOID, POUTBUF );
typedef int  (PWFCAPI PPROCESSINPUTPROC)( LPVOID, LPBYTE, USHORT );
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
typedef INT (PWFCAPI PSENDDATAPROC) (DWORD, USHORT usChannel, LPBYTE pData, USHORT usLen, LPVOID pUserData, UINT32 uiFlags);


/*=============================================================================
 ==   Virtual driver hook structures
 ============================================================================*/

/*
 *  Virtual driver write hook structure
 */
typedef void (PWFCAPI PVDWRITEPROCEDURE)( LPVOID, USHORT, LPBYTE, USHORT );

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

// VDWRITEHOOKEX 
// 
// Used when communicating with HPC (client 13.0) or later 
// 

typedef struct _VDWRITEHOOKEX 
{ 
	USHORT usVersion;						// Version number of HPC client; filled in by WdQueryInformation call 
	PSENDDATAPROC pSendDataProc;			// Pointer to HPC SendData to server API 
	USHORT usExtraLength;					// length of data pointed to by pExtraData 
	LPVOID pExtraData;						// reserved for future expansion 
	HPC_CallTable* pHpcCallTable;			// Address of the HPC Call Table
} VDWRITEHOOKEX, * PVDWRITEHOOKEX; 


/* The current mode of the ICA window */
typedef union _WDICAWINDOWMODE
{
    struct
    {
        UINT Reserved : 1;
        UINT Seamless : 1;
        UINT Panning : 1;
        UINT Scaling : 1;
    } Flags;
    UINT Value;
} WDICAWINDOWMODE;

/* Populated by WdGetICAWindowInfo call */
typedef struct _WDICAWINDOWINFO
{
    HWND hwnd;										/* The ica window handle */
    WDICAWINDOWMODE mode;							/* The current mode of the window */
	UINT32 xWinWidth, yWinHeight, xViewWidth, yViewHeight; /* ICA window dimensions */
    INT xViewOffset, yViewOffset;					/* The window's view offset (i.e panning) */
} WDICAWINDOWINFO, * PWDICAWINDOWINFO;

// WDOVERLAYINFO
//
// Populated by WDAPI_TRANSLATEOVERLAYWINDOW call
//
typedef struct _WDOVERLAYINFO
{
	RECT serverRect; //In Server's window rect
	RECT clientRect; //Out Translated client rect 
	DOUBLE scaleFactor; // Out
	BOOL bScaleToFit; //In Flag whether we also account for scale to fit
	UINT32 xViewOffset, yViewOffset; //Out Window's view offset
} WDOVERLAYINFO, * PWDOVERLAYINFO;


#endif /* __WDAPI_H__ */
