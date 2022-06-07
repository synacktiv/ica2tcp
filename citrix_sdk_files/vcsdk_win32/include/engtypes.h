/***************************************************************************
*
*   engtypes.h
*
*   This module contains basic types defined by the ICA Client Engine.
*
*
*  Copyright 1998-2015 Citrix Systems, Inc.  
*
****************************************************************************/

#ifndef _ENGTYPES_H_
#define _ENGTYPES_H_

#pragma once

#include <icaconst.h>
#include <ica.h>

/*=============================================================================
 ==   assure consistant structure packing regardless of compiler flags
 ============================================================================*/

#ifdef PLATFORM_PRAGMAS
#pragma ENG_PACKING_ENABLE
#endif


/*=============================================================================
 ==   String typedefs (string length + 1 for NULL terminator)
 ============================================================================*/

#ifdef  VERSION1_LABEL
typedef CHAR LABEL[LABEL_LENGTH + 1];
#endif

#ifdef DOMAIN
#undef DOMAIN
#endif

typedef CHAR DOSFILENAME      [ DOSFILENAME_LENGTH       + 1 ];
typedef CHAR DESCRIPTION      [ DESCRIPTION_LENGTH       + 1 ];
typedef CHAR FRIENDLYNAME     [ FRIENDLYNAME_LENGTH      + 1 ];
typedef CHAR VERYLARGENAME    [ VERYLARGENAME_LENGTH     + 1 ];
typedef CHAR PROGRAMGROUP     [ PROGRAMGROUP_LENGTH      + 1 ];
typedef CHAR NAMEENUMERATOR   [ NAMEENUMERATOR_LENGTH    + 1 ];
typedef CHAR NAMERESOLVER     [ NAMERESOLVER_LENGTH      + 1 ];
typedef CHAR TRANSPORTNAME    [ TRANSPORTNAME_LENGTH     + 1 ];
typedef CHAR ENCRYPTIONLEVEL  [ ENCRYPTIONLEVEL_LENGTH   + 1 ];
typedef CHAR EMULATIONNAME    [ EMULATIONNAME_LENGTH     + 1 ];
typedef CHAR CLIENTNAME       [ CLIENTNAME_LENGTH        + 1 ];
typedef CHAR CLIENTSN         [ CLIENTSN_LENGTH          + 1 ];
typedef CHAR CTX_DOMAIN       [ DOMAIN_LENGTH            + 1 ];
typedef CHAR USERNAME         [ USERNAME_LENGTH          + 1 ];
typedef CHAR PASSWORD         [ PASSWORD_LENGTH          + 1 ];
typedef CHAR ENCRYPTEDPASSWORD[ ENCRYPTEDPASSWORD_LENGTH + 1 ];
typedef CHAR DIRECTORY        [ DIRECTORY_LENGTH         + 1 ];
typedef CHAR INITIALPROGRAM   [ INITIALPROGRAM_LENGTH    + 1 ];
typedef CHAR ADDRESS          [ ADDRESS_LENGTH           + 1 ];
typedef CHAR DEVICENAME       [ DEVICENAME_LENGTH        + 1 ];
typedef CHAR BAUD             [ BAUD_LENGTH              + 1 ];
typedef CHAR MODEM            [ MODEM_LENGTH             + 1 ];
typedef CHAR MODEMINIT        [ MODEMINIT_LENGTH         + 1 ];
typedef CHAR MODEMINITSTRING  [ MODEMINITSTRING_LENGTH   + 1 ];
typedef CHAR KEYBOARDLAYOUT   [ KEYBOARDLAYOUT_LENGTH    + 1 ];
typedef CHAR KEYBOARDTYPE     [ KEYBOARDTYPE_LENGTH      + 1 ];
typedef CHAR HOTKEYNAME       [ HOTKEYNAME_LENGTH        + 1 ];
typedef CHAR DIALINGLOCATION  [ DIALINGLOCATION_LENGTH   + 1 ];
typedef CHAR DIALINGPREFIX    [ DIALINGPREFIX_LENGTH     + 1 ];
typedef CHAR DIALPROPENTRY    [ DIALPROPENTRY_LENGTH     + 1 ];
typedef CHAR DIALCOUNTRY      [ DIALCOUNTRY_LENGTH       + 1 ];
typedef CHAR FILEPATH         [ FILEPATH_LENGTH          + 1 ];

typedef ADDRESS ADDRESSLIST[MAX_BROWSERADDRESSLIST];

typedef WCHAR DESCRIPTIONW    [ DESCRIPTION_LENGTH       + 1 ];
typedef WCHAR FRIENDLYNAMEW   [ FRIENDLYNAME_LENGTH      + 1 ];
typedef WCHAR VERYLARGENAMEW  [ VERYLARGENAME_LENGTH     + 1 ];
typedef WCHAR CLIENTNAMEW     [ CLIENTNAME_LENGTH        + 1 ];
typedef WCHAR DOMAINW     [ DOMAIN_LENGTH            + 1 ];
typedef WCHAR USERNAMEW       [ USERNAME_LENGTH          + 1 ];
typedef WCHAR REGIONOIDW      [ MAXOID_LENGTH            + 1 ];
typedef WCHAR INITIALPROGRAMW [ INITIALPROGRAM_LENGTH    + 1 ];
typedef WCHAR ADDRESSW        [ ADDRESS_LENGTH           + 1 ];
    

/*=============================================================================
 ==   UI suppliers must use user-defined messages above WM_WFENG_USER
 ============================================================================*/
#define WM_WFENG_USER (WM_USER+1000)


/*=============================================================================
 ==   WFENG dll entry points
 ============================================================================*/
/********  Not Used *********/
#ifdef  NotUsed
#define WFENG_API_VERSION 1L        /* WinFrame 1.54+ */
#define WFENG_API_VERSION 2L        /* add serial number to wf 1.5 (build 158) */
#define WFENG_API_VERSION 3L        /* WinFrame Client 1.50.67+ */
#define WFENG_API_VERSION 4L        /* WinFrame Internet Client */
#define WFENG_API_VERSION 5L        /* MF 1.8 FR1.9/SP2 Client */
#endif /* NotUsed */

#define WFENG_API_VERSION 6L        /* Delaware Client */


typedef VOID (PWFCAPI PFNUIPOLL)( VOID );
typedef LRESULT (PWFCAPI PFNSTATUSMSGPROC)( HANDLE hWFE,
                   INT message,
                   LPARAM lParam );


/*
 * Data block for WFEngOpen()
 */
typedef struct _WFEOPEN {
   ULONG              Version;           /* ALL API version - set to WFENG_API_VERSION */
   PFNSTATUSMSGPROC   pfnStatusMsgProc;  /* ALL StatusMsgProc() function */
   LPSTR              pszClientname;     /* ALL */
   USHORT             uInitialWidth;     /* WIN (optional) Initial window width */
   USHORT             uInitialHeight;    /* WIN (optional) Initial window height */
   LPSTR              pszLogLabel;       /* WIN (optional) Connection identifier string */
   LPSTR              pszTitle;          /* WIN (optional) Window Title */
   /* below is for API Version 2 and above */
   LPSTR              pszClientSN;       /* can be null pointer */
   /* below is for API Version 3 and above */
   LPSTR              pszIconPath;       /* WIN (optional) Icon Path */
   USHORT             uIconIndex;        /* WIN (optional) Icon Index */
   /* below is for API Version 4 and above */
   ULONG              reserved;          /* Reserved - must be 0 */
   /* below is for API Version 6 and above */
   LPWSTR             pszClientnameW;    /* WIN (optional) obsoletes pszClientname */
   LPWSTR             pszTitleW;         /* WIN (optional) obsoletes pszTitle */
   BOOL				  fPrelaunch;        /* Indicates whether this is a prelaunch app */
} WFEOPEN, FAR *PWFEOPEN;

/*
 * Data block for WFEngLoadxxx()
 */
typedef struct _WFELOAD {
   HANDLE handle;        /* Output - handle to loaded module */
   LPSTR  pszModuleName; /* List of FQ UI modules ('\0\0' terminated for WFEngLoadSession) */
   ULONG  cbIniSectionSize;
   LPVOID pIniSection;
} WFELOAD, FAR *PWFELOAD;

/*
 *  WFEngSetInformation()/WFEngQueryInformation() enum
 */
typedef enum _WFEINFOCLASS
{
#ifndef unix
    WFELastError,                 /* Query */
    WFEAddReadHook,               /* Set   */
    WFERemoveReadHook,            /* Set   */
    WFEThinwireCache,             /* Query */
#endif /* unix */
    WFEWindowHandle,              /* Query */			/* only for VDMM */
#ifndef unix
    WFESetFocus,                  /* Set   */
    WFEKillFocus,                 /* Set   */
#endif /* unix */
    WFERedraw,                    /* Set   */
#ifndef unix
    WFEVdInfo,                    /* Set & Query */
    WFELogInfo,                   /* Set   */
#endif /* unix */
    WFEHotkey,                    /* Set   */
#ifndef unix
    WFEPnLogOff,                  /* Set */
    WFEOwnerData,                 /* Set & Query */
    WFESetProductID,              /* Set   */
    WFERaiseSoftkey,              /* Set   */
    WFELowerSoftkey,              /* Set   */
#endif /* unix */
    WFEClientDataServer,          /* Query */			/* only for ICO */
    WFEClientDataDomain,          /* Query */			/* only for ICO */
    WFEClientDataUsername,        /* Query */			/* only for ICO */
    WFEDesktopInfo,               /* Query */
#ifndef unix
    WFEGraphParams,               /* Set & Query */		/* only for LATER ? */
    WFEScaleStatus,               /* Query */
    WFEAdjustParams,              /* Query */
#endif /* unix */
    WFEInitSessionData,           /* Query */
    WFEIOStatus,                  /* Query */			/* only for ICO */
#ifndef unix
    WFEWndSubLst,                 /* Query */
    WFEbIPCEngine,                /* Query */
#endif /* unix */
    WFEScreenDC,                  /* Set */
#ifndef unix
    WFEGraphMode,                 /* Set & Query */
#endif /* unix */
    WFESetCursor,                 /* Set */
#ifndef unix
    WFEScancodeEvent,             /* Set */
#endif /* unix */
#if defined(UNICODESUPPORT) || !defined(unix)
    WFEUnicodeEvent,              /* Set */
#endif
    WFEHkeyAction,                /* Set & Query */
    WFESetCursorPos,              /* Set */
    WFEInstallPath,               /* Query */
    WFEKeyboardLayout,            /* Query */
#ifndef unix
    WFETextModeStatus,            /* Query */
#endif /* unix */
    WFETWIMode,                   /* Query */			/* only for VDMM */
#ifndef unix
    WFEScale,                     /* Set & Query */
#endif /* unix */
    WFEReconnectInfo,             /* Set & Query */
	WFETransportProtocolType,     /* Query */
#ifndef unix
    WFESession,                   /* Set & Query */
#endif /* unix */
    WFEMaxServerLVB,              /* Set */
#ifndef unix
    WFEEnoughServerMem,           /* Query */
    WFEMultiMonSupport,           /* Query */
    WFECancelReconnect,           /* Set */
#endif /* unix */
    WFEDegradeColorFirst,         /* Set */
#ifndef unix
    WFEDisplayCaps,               /* Set & Query */
    WFEMonitorLayout,             /* Query */
    WFESSLCachedSessionStrength,  /* Query */
#endif /* unix */
//#ifdef TWI_INTERFACE_ENABLED
    WFEIPCStatus,                 /* Set */
    WFESeamlessEnabled,           /* Set */
    WFESeamlessStatusChanged,     /* Set */
	WFEStatusDlgHandle,			  /* Set & Query */
	WFEEuemStartupTimes,	      /* Set */
	WFELicensedFeatures,          /* Query */
    WFEAppendUsername,             /* Set */
	WFESetMonitorLayout,		  /* Set */
	WFEFullScreenMode,			  /* Query */
	WFEGetMonitorLayout,		  /* Query */
//#endif
#ifndef unix
    WFESetHTC,                    /* Set */
    WFEStatusUseICO,              /* Set */
    WFELanguageBar,               /* Set */
    WFEngineIPCWnd,              /* Query */
    WFLocalIPCWnd,               /* Query */
    WFVdHandleByName,             /*Query*/
    WFEAuthorizationDecision,     /* Set & Query */
    WFEAuthorizationDecisionUpdateDriver,     /* Set & Query */
    WFEAuthorizationProposal,     /* Set */
    WFECloseSecurityPopups,       /* Set */
#endif
	WFEStartFastPoll,              /* Set */
    WFEStatusDlgCookie,           /* Set & Query */
#ifndef unix
    WFEConnectQuery,               /* Set */
#endif
	WFEVdToCcmInfo,             // a Vd is sending information to CCM
	WFECcmSessionId,            // Set the Engine's SessionId that was assigned by CCM
    WFCEngToCcmInfo,             // The engine is sending information to CCM
	WFENewUrlRedirectionEnabled, //sets newUrlRedirection property so that CCM could grab it.
    WFEVdToEngSessionGUID,
#ifndef unix
	WFEDimWindow,
#endif
    WFEReverseSeamlessEnabled,  			    // Set & Query (Set to enable/disable reverse seamless. Query to check if reverse seamless is enabled)
	WFESuppressSRNotification				/* Query */
} WFEINFOCLASS;


/*
 * Data block for WFESetProductID
 */
typedef struct _WFEPRODUCTID {
    USHORT  ProductID;
} WFEPRODUCTID, * PWFEPRODUCTID;


/*
 * Data block for WFESetClientInfo
 */

/*
 * Data block for WFEVdInfo via WFEngSetInformation/QueryInformation
 */
typedef struct _WFEVDINFO {
    HANDLE hVd;                  /* Vd handle returned from WFEngLoadVd */
    ULONG  type;                 /* Vd-specific info type */
    UINT   cbData;               /* size (in bytes) of pData */
    UCHAR  pData[1];             /* Info buffer - Set (input), Query (output) */
} WFEVDINFO, * PWFEVDINFO;

// Data block for WFEVdToCcmInfo, used to send information from a Vd to CCM
typedef struct _WFEVDTOCCMINFO
{
	ULONG	ulSessionId;	// Engine's SessionId assigned by CCM
	ULONG	type;			// Type of Message
	UINT	cbData;			// Size of pData in bytes
    VIRTUALNAME acVDName;   //Name of the virtual channel
	UCHAR	abData[1];		// Data buffer
} WFEVDTOCCMINFO, * PWFEVDTOCCMINFO;

// Data block for WFEEngToCcInfo, used to send information from the engine to CCM
typedef struct _WFEENGTOCCMINFO
{
    ULONG   ulSessionId;
    USHORT  type;
    USHORT  reserved;
    UINT    cbData;
    UCHAR   abData[1];
}WFEENGTOCCMINFO, *PWFEENGTOCCMINFO;

/* Thinwire VD setinfo type */
#define VdThinWireDeallocateCache   201
#define VdThinWireSaveVideoRegs     202
#define VdThinWireRestoreVideoRegs  203



/* hotkey structure */
typedef struct _ICAENG_HOTKEYINFO {
    UINT16  uiHotkeyID;      /* ID of the Hotkey.*/
    UINT16  uiKeyCode;       /* Keycode (could be scan or unicode) associated with the hot key*/
    UINT16  uiShiftState;    /* Shift state associated with hot key*/
    PUCHAR  pKeyCodeSequence;/* Optional key code sequence to be sent to the server.*/
    UINT16  uiLength;        /* Number of bytes to be sent*/
    UINT16  uiSequenceType;  /* Signifies if the Sequence is Unicode or ScanCode */
#define SCANCODE_SEQUENCE 1  /* Sequence is of type ScanCode. */
#define UNICODE_SEQUENCE  2  /* Sequence is of type UniCode. */
#define CHARCODE_SEQUENCE 3  /* Sequence is of type CharCode. */

    UINT16  uiShiftStateFixed;

#define ALL_STATE   0x0000  /* any shift state is acceptable. No special processing needed*/
#define ALT_STATE   0x0001  /* Server should get only Alt state down. */
#define CTRL_STATE  0x0002  /* Server should get only Ctrl state down. */
#define SHIFT_STATE 0x0004  /* Server should get only Shift state down. */
    PFNDELIVER  pHkeyFunction; /* If sequence not present, call pHkeyFunction*/
} ICAENG_HOTKEYINFO, * PICAENG_HOTKEYINFO;

enum TransportProtocolType
{
    TransportProtocolType_TCP = 1,         // TCP
    TransportProtocolType_UDP             // UDP
};

typedef struct _ICAENG_SCANCODEEVENT
{
    UINT16 cScanCodes;
    PUINT8 pScanCodes;
    UINT16 uiRepeat;
    UINT16 uiShiftState;
}
ICAENG_SCANCODEEVENT, * PICAENG_SCANCODEEVENT;



typedef struct ICAENG_UNICODEEVENT
{
    UINT16          cUnicodeCodes;
    UINT8           uiFlags;               /* the type of char value */
#define WND_UNICODECHAR_KEYUP   0x01
#define WND_UNICODECHAR_SPECIAL 0x02
    UINT16          uiValue;               /* Unicode or special key value */
    UINT16          uiRepeat;
    UINT16          uiShiftState;
}
ICAENG_UNICODEEVENT, * PICAENG_UNICODEEVENT;

/*
 * Data block for WFEngSetInformation-WFELogInfo
 */
typedef struct _WFELOGINFO {
    LPSTR  pszLogFile;
    ULONG  ulLogClass;
    ULONG  ulLogEvent;
    ULONG  ulLogFlags;
} WFELOGINFO, * PWFELOGINFO;

/* 
* Maximum size for WFEngSetInformation-WFEAuthorizationDecision and WFEngSetInformation-WFEAuthorizationDecision
*/
#define MAX_AUTHDECISION 4096

/*
 *  Data Block for WFEngQueryInformation() - WFELastError
 */
typedef struct _WFELASTERROR {
    DOSFILENAME DriverName;
    int Error;
    char Message[ MAX_ERRORMESSAGE ];
} WFELASTERROR, * PWFELASTERROR;

/*
 * Data Block for WFEngQuery/SetInformation - WFEScale
 */
typedef struct _WFESCALE {

    /*
     * Header section
     * Size and version allow for different parameters in the future
     */
    UINT16  uiSize;
    UINT16  uiVersion;
    UINT16  uiCommand;

    /*
     * Parameters area for commands
     */
    UINT16  uiPercent;                              /* used with SCALE_PERCENT */
    UINT16  uiWidth;                                /* used with SCALE_SIZE    */
    UINT16  uiHeight;                               /* used with SCALE_SIZE    */

} WFESCALE, * PWFESCALE;

#define WFESCALE_SIZE               sizeof(WFESCALE)

#define WFESCALE_VERSION_ONE        1
#define WFESCALE_VERSION_CURRENT    WFESCALE_VERSION_ONE

#define WFESCALE_COMMAND_SCALE_DISABLE      0       /* (set) disable scaling                */
#define WFESCALE_COMMAND_SCALE_ENABLE       1       /* (set) enable scaling                 */
#define WFESCALE_COMMAND_SCALE_PERCENT      2       /* (get/set) based on percent           */
#define WFESCALE_COMMAND_SCALE_SIZE         3       /* (get/set) based on width and height  */
#define WFESCALE_COMMAND_SCALE_UP           4       /* (set) to a larger size               */
#define WFESCALE_COMMAND_SCALE_DOWN         5       /* (set) to a smaller size              */
#define WFESCALE_COMMAND_SCALE_DIALOG       6       /* (set) display the scale dialog box   */
#define WFESCALE_COMMAND_SCALE_TO_FIT       7       /* (set) to fit embedded window         */


/*
 * Data Block for WFEngQuery/SetInformation - WFESession
 */
typedef struct _WFESESSION
{
    UINT32  cbWFESession;
    UINT16  uiVersion;
    UINT16  uiRequestType;

    UINT32  cbData;
    UINT32  uiDataOffset;
    /*
     * Version 1 of this structure ends here
     */

} WFESESSION, *PWFESESSION;

#define WFESESSION_SIZE             sizeof(WFESESSION)

#define WFESESSION_VERSION_ONE      1
#define WFESESSION_VERSION_CURRENT  WFESESSION_VERSION_ONE

#define WFESESSION_REQUEST_ATTACH               0
#define WFESESSION_REQUEST_DETACH               1
#define WFESESSION_REQUEST_FREEWFEICOBLOCK      2
#define WFESESSION_REQUEST_HIDEWINDOW           3
#define WFESESSION_REQUEST_HOOKENGINE           4
#define WFESESSION_REQUEST_MODE                 5
#define WFESESSION_REQUEST_PARENTWINDOW         6
#define WFESESSION_REQUEST_SESSIONCACHETIMELEFT 7
#define WFESESSION_REQUEST_SESSIONCACHETIMEOUT  8
#define WFESESSION_REQUEST_SESSIONID            9
#define WFESESSION_REQUEST_SHOWWINDOW           10
#define WFESESSION_REQUEST_STARTTIMEOUT         11
#define WFESESSION_REQUEST_STOPTIMEOUT          12
#define WFESESSION_REQUEST_UNHOOKENGINE         13
#define WFESESSION_REQUEST_WFEENGINFO           14
#define WFESESSION_REQUEST_WFEICOBLOCK          15
#define WFESESSION_REQUEST_STATUSLIST           16

/*
 * Headless requests
 */
#define WFESESSION_REQUEST_HEADLESS_MODE        17
#define WFESESSION_REQUEST_HEADLESS_RENDER      18
#define WFESESSION_REQUEST_MARSHALLED_INTERFACE 19
#define WFESESSION_REQUEST_SECURITY             20

#define WFESESSION_REQUEST_GROUPID              21
#define WFESESSION_REQUEST_UIIPCHANDLE          22
#define WFESESSION_REQUEST_STATUSMESSAGE        23
#define WFESESSION_REQUEST_FLAGS                24

/*
 * XD Security Setting request
 */
#define WFESESSION_REQUEST_SECURITYSETTING      25

/*
 * Used in WFESession structures
 */
#define WFE_MAX_MEMORYNAME_LEN      128
#define WFE_MAX_MUTEXNAME_LEN       128
#define WFE_MAX_SESSIONID_LEN       256
#define WFE_MAX_SESSIONGROUPID_LEN  256


/*
 * Engine modes used with WFEENGINFO.uiMode
 */
#define ENGINE_MODE_UNKNOWN     0
#define ENGINE_MODE_ATTACHED    1
#define ENGINE_MODE_DETACHED    2

/*
 * Engine Information structure
 */
typedef struct _WFEENGINFO
{
    UINT32  cbWFEEngInfo;
    UINT16  uiVersion;
    UINT32  uiTimeout;
    UINT32  uiTimeleft;
    UINT32  uiMode;
    HANDLE  hParentWindow;
    UINT32  cbSessionId;
    CHAR    szSessionId[WFE_MAX_SESSIONID_LEN];
    UINT32  cbMutexName;
    CHAR    szMutexName[WFE_MAX_MUTEXNAME_LEN];
    /*
     * Version 1 of this structure ends here
     */
    UINT32  cbGroupId;
    CHAR    szGroupId[WFE_MAX_SESSIONGROUPID_LEN];
    UINT32  uiFlags;
    /*
     * Version 2 ends here
     */

} WFEENGINFO, *PWFEENGINFO;

#define WFEENGINFO_SIZE             sizeof(WFEENGINFO)

#define WFEENGINFO_VERSION_ONE      1
#define WFEENGINFO_VERSION_TWO      2
#define WFEENGINFO_VERSION_CURRENT  WFEENGINFO_VERSION_TWO


/*
 * WFEICOBLOCK structure definition
 */
typedef struct _WFEICOBLOCK
{
    UINT32 cbWFEICOBlock;
    UINT16 uiVersion;
    UINT32 cbMemoryName;
    CHAR   szMemoryName[WFE_MAX_MEMORYNAME_LEN];
    UINT32 cbMemory;
} WFEICOBLOCK, *PWFEICOBLOCK;

#define WFEICOBLOCK_SIZE             sizeof(WFEICOBLOCK)

#define WFEICOBLOCK_VERSION_ONE      1
#define WFEICOBLOCK_VERSION_CURRENT  WFEICOBLOCK_VERSION_ONE

/*
 * Engine Attach structure
 */
typedef struct _WFEENGATTACH
{
    UINT32  cbWFEEngAttach;
    UINT16  uiVersion;

    HANDLE  hParentWindow;
    /*
     * Version 1 of this structure ends here
     */

} WFEENGATTACH, *PWFEENGATTACH;

#define WFEENGATTACH_SIZE             sizeof(WFEENGATTACH)

#define WFEENGATTACH_VERSION_ONE      1
#define WFEENGATTACH_VERSION_CURRENT  WFEENGATTACH_VERSION_ONE

/*
 * Engine Detach structure
 */
typedef struct _WFEENGDETACH
{
    UINT32  cbWFEEngDetach;
    UINT16  uiVersion;

    UINT32  uiTimeout;

    UINT32  cbSessionId;
    CHAR    szSessionId[WFE_MAX_SESSIONID_LEN];
    /*
     * Version 1 of this structure ends here
     */

} WFEENGDETACH, *PWFEENGDETACH;

#define WFEENGDETACH_SIZE             sizeof(WFEENGDETACH)

#define WFEENGDETACH_VERSION_ONE      1
#define WFEENGDETACH_VERSION_CURRENT  WFEENGDETACH_VERSION_ONE


typedef struct _WFECACHEDEVENTITEM
{
    UINT32 uiCachedStatusMessageID;
    UINT32 nCount;

} WFECACHEDEVENTITEM, *PWFECACHEDEVENTITEM;

#define WFECACHEDEVENTITEM_SIZE             sizeof(WFECACHEDEVENTITEM)


typedef struct _WFECACHEDEVENTS
{
    UINT32  cbWFESessionCachedEvents;
    UINT16  uiVersion;
    UINT32  nEventItems;

    UINT32  cbData;
    UINT32  uiDataOffset;

    /*
     * Version 1 of this structure ends here.  WFECHACHEDEVENTITEM  follow
     */

} WFECACHEDEVENTS, *PWFECACHEDEVENTS;

#define WFECACHEDEVENTS_SIZE             sizeof(WFECACHEDEVENTS)

#define WFECACHEDEVENTS_VERSION_ONE      1
#define WFECACHEDEVENTS_VERSION_CURRENT  WFECACHEDEVENTS_VERSION_ONE

typedef struct _WFESESSION_MARSHALLED_INTERFACE
{
    UINT32  cbMarshalledInterface;  /* sizeof(_WFE_SESSION_MARSHALLED_INTERFACE) */
    UINT16  uiVersion;              /* Version  */
    UINT32  cbData;                 /* Size of returned data */
    CHAR    data[1];                /* Returned data */
} WFESESSION_MARSHALLED_INTERFACE, *PWFESESSION_MARSHALLED_INTERFACE;

/*
 * Flags used with WFESESSION_REQUEST_SECURITY
 */
#define SECURITY_FLAG_DISABLE_KEYBOARD_EVENTS    0x00000001
#define SECURITY_FLAG_DISABLE_SCREEN_SHOT        0x00000002
#define SECURITY_FLAGS_DEFAULT                   0

#define WFESESSION_SECURITY_VERSION_ONE      1
#define WFESESSION_SECURITY_VERSION_CURRENT  WFESESSION_SECURITY_VERSION_ONE

typedef struct _WFESESSION_SECURITY
{
    UINT32  cbSessionSecurity;
    UINT16  uiVersion;

    UINT32  uiSecurityFlags;

} WFESESSION_SECURITY, *PWFESESSION_SECURITY;


#define WFESESSION_STATUSMESSAGE_VERSION_ONE      1
#define WFESESSION_STATUSMESSAGE_VERSION_CURRENT  WFESESSION_STATUSMESSAGE_VERSION_ONE

typedef struct _WFESESSION_STATUSMESSAGE
{
    UINT32  cbSize;
    UINT16  uiVersion;

    UINT32  uiStatusMessage;

} WFESESSION_STATUSMESSAGE, *PWFESESSION_STATUSMESSAGE;

#define WFESESSION_FLAGS_VERSION_ONE      1
#define WFESESSION_FLAGS_VERSION_CURRENT  WFESESSION_FLAGS_VERSION_ONE

#define WFESESSION_FLAG_EMBEDDED          0
#define WFESESSION_FLAG_LAUNCHED          1

typedef struct _WFESESSION_FLAGS
{
    UINT32  cbSize;
    UINT16  uiVersion;

    UINT32  uiFlags;

} WFESESSION_FLAGS, *PWFESESSION_FLAGS;


#ifdef PLATFORM_PRAGMAS
#pragma ENG_PACKING_RESTORE
#endif


/*
 * Data Block for SetInformation - WFEIPCStatus
 */
typedef struct _IPCSTATUSMSG {
   HANDLE hWFE;
   INT  message;
   LPARAM lParam;
} IPCSTATUSMSG, *PIPCSTATUSMSG;

#define WFENGMONITOR_QUERY_LAYOUT				    0
#define WFENGMONITOR_QUERY_LAYOUT_WINDOW		    1	// only valid for DesiredWinType==16
#define WFENGMONTIOR_QUERY_LAYOUT_INITIALIZE	    2	// only valid for DesiredWinType==16
#define WFENGMONITOR_QUERY_LAYOUT_WND_CHECK_CAPTION	3
#define WFENGMONITOR_QUERY_LAYOUT_OSSCALEFACTOR	    4	// currently only implemented for single monitor sessions

/*
 * Security Settings Structure
 */
typedef struct _WFEFSECURITYSETTINGDATA
{
    UINT32 type; // Realy a CCMSecurityOption
    UINT32 secSetting; // made up of WFESECURITYSETTING_...
} WFEFSECURITYSETTINGDATA, *PWFEFSECURITYSETTINGDATA;

#define WFESECURITYSETTINGSIZE  sizeof(WFEFSECURITYSETTINGDATA)
// The following defines need to match those in XenDesktop\...\AccessSettings.cs
// First two bits
#define WFESECURITYSETTING_PROMPT     0
#define WFESECURITYSETTING_READ       1
#define WFESECURITYSETTING_READ_WRITE 2
#define WFESECURITYSETTING_NONE       3
#define WFESECURITYSETTING_MASK       3
// Third bit
#define WFESECURITYSETTING_PERSISTENT 4
// Fourth bit
#define WFESECURITYSETTING_DISABLED  8
// Fifth bit
#define WFESECURITYSETTING_DISABLE_PERSISTENCE 16
#endif /* _ENGTYPES_H_ */
