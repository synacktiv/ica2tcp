/*******************************************************************************
*
* WFAPI.H
*
*   Citrix server public APIs
*
*
* Citrix hereby permits you to use, reproduce, modify, display, compile,
* sublicense and distribute all or any portion of the sample code contained
* in this file, provided that you remove this Copyright block from any
* in this file, provided that you remove this Modifications Copyright 1999-2010 Citrix Systems, Inc.
* redistributed versions of this file.
*
* THIS SAMPLE CODE IS PROVIDED BY CITRIX "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
*
*******************************************************************************/

#ifndef _INC_WFAPI
#define _INC_WFAPI

#include <wtypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
==   Defines
==============================================================================*/

/*
 *  Specifies the current server
 */
#define WF_CURRENT_SERVER         ((HANDLE)NULL)
#define WF_CURRENT_SERVER_HANDLE  ((HANDLE)NULL)
#define WF_CURRENT_SERVER_NAME    (NULL)

/*
 *  Specifies the current session (SessionId)
 */
#define WF_CURRENT_SESSION ((DWORD)-1)

/*
 *  Possible pResponse values from WFSendMessage()
 */
#ifndef IDTIMEOUT
#define IDTIMEOUT 32000
#endif
#ifndef IDASYNC
#define IDASYNC   32001
#endif

/*
 *  Shutdown flags
 */
#define WF_WSD_LOGOFF      0x00000001  // log off all users except
                                       // current user; deletes
                                       // WinStations (a reboot is
                                       // required to recreate the
                                       // WinStations)
#define WF_WSD_SHUTDOWN    0x00000002  // shutdown system
#define WF_WSD_REBOOT      0x00000004  // shutdown and reboot
#define WF_WSD_POWEROFF    0x00000008  // shutdown and power off (on
                                       // machines that support power
                                       // off through software)
#define WF_WSD_FASTREBOOT  0x00000010  // reboot without logging users
                                       // off or shutting down

#define WF_NUM_REGION_RECTS    16           // Maximum number of region rectangles
                                       // sent in a window tracking message

/*
 *  SMC Specific defines (refer to CtxSmcApi.h)
 */
#include <CtxSmcApi.h>

#define WF_SMCAPI_VERSION_NUMBER            CTXSMCAPI_VERSION_NUMBER
#define WF_SMCAPI_VERSION_NUMBER_V2         CTXSMCAPI_VERSION_NUMBER_V2

#define WF_SMC_COMPATIBILITY_XP             SMC_API_COMPATIBILITY_XP

#define WF_SMC_COMPATIBILITY_XPE_FR1        SMC_API_COMPATIBILITY_XPE_FR1

#define WF_SMC_SESSION_VC_PRIORITY_VERYHIGH CTX_SESSION_VC_PRIORITY_VERYHIGH
#define WF_SMC_SESSION_VC_PRIORITY_HIGH     CTX_SESSION_VC_PRIORITY_HIGH
#define WF_SMC_SESSION_VC_PRIORITY_NORMAL   CTX_SESSION_VC_PRIORITY_NORMAL
#define WF_SMC_SESSION_VC_PRIORITY_LOW      CTX_SESSION_VC_PRIORITY_LOW

#define WF_SMC_LATENCY_INVALID   (ULONG)    -1

/*===================================================================
==   WF_CONNECTSTATE_CLASS - Session connect state
=====================================================================*/

typedef enum _WF_CONNECTSTATE_CLASS
{
    WFActive,              // User logged on to WinStation
    WFConnected,           // WinStation connected to client
    WFConnectQuery,        // In the process of connecting to client
    WFShadow,              // Shadowing another WinStation
    WFDisconnected,        // WinStation logged on without client
    WFIdle,                // Waiting for client to connect
    WFListen,              // WinStation is listening for connection
    WFReset,               // WinStation is being reset
    WFDown,                // WinStation is down due to error
    WFInit                 // WinStation in initialization
} WF_CONNECTSTATE_CLASS;


/*===================================================================
==   WF_PROTOCOL - Protocol type
=====================================================================*/

typedef enum _WF_PROTOCOL
{
    WFProtocolNone,
    WFProtocolICA,
    WFProtocolRDP
} WF_PROTOCOL;
      



/*==============================================================================
==   WF_SERVER_INFO - returned by WFEnumerateServers (version 1)
==============================================================================*/

/*
 *  WFEnumerateServers() returns two variables: pServerInfo and Count.  The
 *  latter is the number of WF_SERVER_INFO structures contained in the former.
 *  In order to read each server, iterate i from 0 to Count - 1 and reference
 *  the server name as pServerInfo[i].pServerName; for example:
 *
 *  for (i = 0; i < Count; i++)
 *      _tprintf(TEXT("%s "), pServerInfo[i].pServerName);
 *
 *  The memory returned looks like the following.  P is a pServerInfo
 *  pointer, and D is the string data for that pServerInfo:
 *
 *  P1 P2 P3 P4 ... Pn D1 D2 D3 D4 ... Dn
 *
 *  This makes it easier to iterate the servers, using code similar to
 *  the above.
 */

typedef struct _WF_SERVER_INFOW
{
    LPWSTR pServerName;    // server name
} WF_SERVER_INFOW, * PWF_SERVER_INFOW;

typedef struct _WF_SERVER_INFOA
{
    LPSTR pServerName;     // server name
} WF_SERVER_INFOA, * PWF_SERVER_INFOA;

#ifdef UNICODE
#define WF_SERVER_INFO  WF_SERVER_INFOW
#define PWF_SERVER_INFO PWF_SERVER_INFOW
#else
#define WF_SERVER_INFO  WF_SERVER_INFOA
#define PWF_SERVER_INFO PWF_SERVER_INFOA
#endif


/*==============================================================================
==   WF_SESSION_INFO - returned by WFEnumerateSessions (version 1)
==============================================================================*/

/*
 *  WFEnumerateSessions() returns data in a similar format to the above
 *  WFEnumerateServers().  It returns two variables: pSessionInfo and Count.
 *  The latter is the number of WF_SESSION_INFO structures contained in the
 *  former.  Iteration is similar, except that there are three parts to each
 *  entry, so it would look like this:
 *
 *  for (i = 0; i < Count; i++)
 *  {
 *      _tprintf(TEXT("%-5u  %-20s  %u\n"),
 *               pSessionInfo[i].SessionId,
 *               pSessionInfo[i].pWinStationName,
 *               pSessionInfo[i].State);
 *  }
 *
 *  The memory returned is also segmented as the above, with all the
 *  structures allocated at the start and the string data at the end.
 *  We'll use S for the SessionId, P for the pWinStationName pointer
 *  and D for the string data, and C for the connect State:
 *
 *  S1 P1 C1 S2 P2 C2 S3 P3 C3 S4 P4 C4 ... Sn Pn Cn D1 D2 D3 D4 ... Dn
 *
 *  As above, this makes it easier to iterate the sessions.
 */

typedef struct _WF_SESSION_INFOW
{
    DWORD                 SessionId;       // session id
    LPWSTR                pWinStationName; // name of WinStation to which this
                                           // session is connected
    WF_CONNECTSTATE_CLASS State;           // connection state (see enum)
} WF_SESSION_INFOW, * PWF_SESSION_INFOW;

typedef struct _WF_SESSION_INFOA
{
    DWORD                 SessionId;       // session id
    LPSTR                 pWinStationName; // name of WinStation to which this
                                           // session is connected
    WF_CONNECTSTATE_CLASS State;           // connection state (see enum)
} WF_SESSION_INFOA, * PWF_SESSION_INFOA;


#ifdef UNICODE
#define WF_SESSION_INFO  WF_SESSION_INFOW
#define PWF_SESSION_INFO PWF_SESSION_INFOW
#else
#define WF_SESSION_INFO  WF_SESSION_INFOA
#define PWF_SESSION_INFO PWF_SESSION_INFOA
#endif


/*==============================================================================
==   WF_PROCESS_INFO - returned by WFEnumerateProcesses (version 1)
==============================================================================*/

/*
 *  WFEnumerateProcesses() also returns data similar to WFEnumerateServers().
 *  It returns two variables: pProcessInfo and Count.  The latter is the number
 *  of WF_PROCESS_INFO structures contained in the former.  Iteration is
 *  similar, except that there are four parts to each entry, so it would look
 *  like this:
 *
 *  for (i = 0; i < Count; i++)
 *  {
 *      GetUserNameFromSid(pProcessInfo[i].pUserSid, UserName,
 *                         sizeof(UserName));
 *      _tprintf(TEXT("%-5u %-20s %-5u %s\n"),
 *               pProcessInfo[i].SessionId,
 *               UserName,
 *               pProcessInfo[i].ProcessId,
 *               pProcessInfo[i].pProcessName);
 *  }
 *
 *  The memory returned is also segmented as the above, with all the structures
 *  allocated at the start and the string data at the end.  We'll use S for the
 *  SessionId, R for the ProcessId, P for the pProcessName pointer and D for the
 *  string data, and U for pUserSid:
 *
 *  S1 R1 P1 U1 S2 R2 P2 U2 S3 R3 P3 U3 ... Sn Rn Pn Un D1 D2 D3 ... Dn
 *
 *  As above, this makes it easier to iterate the processes.
 */

typedef struct _WF_PROCESS_INFOW
{
    DWORD  SessionId;    // session id
    DWORD  ProcessId;    // process id
    LPWSTR pProcessName; // name of process
    PSID   pUserSid;     // user's SID
} WF_PROCESS_INFOW, * PWF_PROCESS_INFOW;

typedef struct _WF_PROCESS_INFOA
{
    DWORD SessionId;     // session id
    DWORD ProcessId;     // process id
    LPSTR pProcessName;  // name of process
    PSID  pUserSid;      // user's SID
} WF_PROCESS_INFOA, * PWF_PROCESS_INFOA;

#ifdef UNICODE
#define WF_PROCESS_INFO  WF_PROCESS_INFOW
#define PWF_PROCESS_INFO PWF_PROCESS_INFOW
#else
#define WF_PROCESS_INFO  WF_PROCESS_INFOA
#define PWF_PROCESS_INFO PWF_PROCESS_INFOA
#endif

/*==============================================================================
==   WF_PROCESS_INFOEX - returned by WFEnumerateProcessesEx (version 1)
==============================================================================*/

/*
 *  WFEnumerateProcessesEx() returns data similar to WFEnumerateProcesses().  It
 *  returns two variables: pProcessInfoEx and Count.  The latter is the number
 *  of WF_PROCESS_INFOEX structures contained in the former.  Iteration is
 *  similar, except there are a lot more entries for each data block:
 *
 *  for (i = 0; i < Count; i++)
 *  {
 *      GetUserNameFromSid(pProcessInfo[i].pUserSid, UserName,
 *                         sizeof(UserName));
 *      _tprintf(TEXT("%-5u %-20s %-5u %s\n"),
 *               pProcessInfo[i].SessionId,
 *               UserName,
 *               pProcessInfo[i].ProcessId,
 *               pProcessInfo[i].pProcessName);
 *  }
 *
 *  The memory returned is also segmented as the above, with all the structures
 *  allocated at the start and the string data at the end.  We'll use S for the
 *  SessionId, R for the ProcessId, P for the pProcessName pointer and D for the
 *  string data, and U for pUserSid:
 *
 *  S1 R1 P1 U1 S2 R2 P2 U2 S3 R3 P3 U3 ... Sn Rn Pn Un D1 D2 D3 ... Dn
 *
 *  As above, this makes it easier to iterate the processes.
 */

typedef struct _WF_PROCESS_INFOEXW
{
    DWORD         SessionId;    // session id
    DWORD         ProcessId;    // process id
    LPWSTR        pProcessName; // name of process
    PSID          pUserSid;     // user's SID
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    SIZE_T        WorkingSetSize;
    SIZE_T        PeakWorkingSetSize;
    SIZE_T        VirtualSize;
    SIZE_T        PeakVirtualSize;
    ULONG         NumThreads;
    ULONG         NumHandles;
} WF_PROCESS_INFOEXW, * PWF_PROCESS_INFOEXW;

typedef struct _WF_PROCESS_INFOEXA
{
    DWORD         SessionId;    // session id
    DWORD         ProcessId;    // process id
    LPSTR         pProcessName; // name of process
    PSID          pUserSid;     // user's SID
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    SIZE_T        WorkingSetSize;
    SIZE_T        PeakWorkingSetSize;
    SIZE_T        VirtualSize;
    SIZE_T        PeakVirtualSize;
    ULONG         NumThreads;
    ULONG         NumHandles;
} WF_PROCESS_INFOEXA, * PWF_PROCESS_INFOEXA;

#ifdef UNICODE
#define WF_PROCESS_INFOEX  WF_PROCESS_INFOEXW
#define PWF_PROCESS_INFOEX PWF_PROCESS_INFOEXW
#else
#define WF_PROCESS_INFOEX  WF_PROCESS_INFOEXA
#define PWF_PROCESS_INFOEX PWF_PROCESS_INFOEXA
#endif

/*==============================================================================
==   WF_INFO_CLASS - WFQuerySessionInformation
==    (See additional typedefs for more info on structures)
==============================================================================*/

typedef enum _WF_INFO_CLASS
{
    WFVersion,             // OSVERSIONINFO
    WFInitialProgram,
    WFWorkingDirectory,
    WFOEMId,
    WFSessionId,
    WFUserName,
    WFWinStationName,
    WFDomainName,
    WFConnectState,
    WFClientBuildNumber,
    WFClientName,
    WFClientDirectory,
    WFClientProductId,
    WFClientHardwareId,
    WFClientAddress,
    WFClientDisplay,
    WFClientCache,
    WFClientDrives,
    WFICABufferLength,
    WFLicenseEnabler,
    RESERVED2,
    WFApplicationName,
    WFVersionEx,
    WFClientInfo,
    WFUserInfo,
    WFAppInfo,
    WFClientLatency,
    WFSessionTime,
    WFLicensingModel
} WF_INFO_CLASS;


/*==============================================================================
==   WFQuerySessionInformation - (WFClientProductId)
==
==   NOTE: the following are Citrix-defined client product IDs.  OEM clients
==         will have different, OEM-specific values.
==============================================================================*/

#define CLIENTID_CITRIX_DOS        0x0001  // citrix dos client
#define CLIENTID_CITRIX_TEXT_TERM  0x0004  // citrix text terminals
#define CLIENTID_CITRIX_DIRECTICA  0x0007  // citrix DirectICA clients


/*==============================================================================
==   WFQuerySessionInformation - (WFClientAddress)
==============================================================================*/

typedef struct _WF_CLIENT_ADDRESS
{
    DWORD AddressFamily;  // AF_INET, AF_IPX, AF_NETBIOS, AF_UNSPEC
    BYTE  Address[20];    // client network address
} WF_CLIENT_ADDRESS, * PWF_CLIENT_ADDRESS;


/*==============================================================================
==   WFQuerySessionInformation - (WFClientDisplay)
==============================================================================*/

typedef struct _WF_CLIENT_DISPLAY
{
    DWORD HorizontalResolution; // horizontal dimensions, in pixels
    DWORD VerticalResolution;   // vertical dimensions, in pixels
    DWORD ColorDepth;           // 1=16, 2=256, 4=64K, 8=16M
} WF_CLIENT_DISPLAY, * PWF_CLIENT_DISPLAY;


/*==============================================================================
==   WFQuerySessionInformation - (WFClientCache)
==============================================================================*/

typedef struct _WF_CLIENT_CACHE
{
    DWORD CacheTiny;          // size of cache for small objects (32 bytes)
    DWORD CacheLowMem;        // size of cache (low memory for DOS;
                              //                virtual memory for Win16/Win32)
    DWORD CacheXms;           // size of cache in XMS memory (DOS only)
    DWORD CacheDisk;          // size of cache in disk (DOS only)
    DWORD DimCacheSize;       // persistent cache size
    DWORD DimBitmapMin;       // minimum bitmap size for persistent cache
    DWORD DimSignatureLevel;  // version of persistent caching
    DWORD DimFilesysOverhead; // The amount of overhead that the file system on
                              //     the client requires per bitmap
} WF_CLIENT_CACHE, * PWF_CLIENT_CACHE;

/*==============================================================================
==   WFQuerySessionInformation - (WFClientDrives)
==============================================================================*/

typedef struct _WF_DRIVES
{
    DWORD DriveLetter; // in ASCII format, i.e. 'C'
    DWORD Flags;       // see below for flag values
} WF_DRIVES, * PWF_DRIVES;

/*
 *  Client drive mapping flags (Flags)
 */
#define WF_DRIVE_REMOVEABLE      0x0001
#define WF_DRIVE_FIXED           0x0002
#define WF_DRIVE_REMOTE          0x0004
#define WF_DRIVE_CDROM           0x0008

typedef struct _WF_CLIENT_DRIVES
{
    DWORD fAutoClientDrives : 1; // are client drives auto-connected?
    DWORD Count;                 // number of actual client drives
    WF_DRIVES Drives[26];        // client drives (room for 26 letters)
} WF_CLIENT_DRIVES, * PWF_CLIENT_DRIVES;

/*==============================================================================
==   WFQuerySessionInformation - (WFClientLatency)
==============================================================================*/

/*
 * Error codes returned for the client latency query.
 *
 * ERROR_SUCCESS        Call succeeded and data is valid.
 * ERROR_INVALID_DATA   Client is capable of providing latency information but
 *                      data is invalid.
 * ERROR_NOT_SUPPORTED  The client does not support latency monitoring.
 */

typedef struct _WF_CLIENT_LATENCY
{
    DWORD Average;              /* Average latency in ms.           */
    DWORD Last;                 /* Last latency in ms.              */
    DWORD Deviation;            /* Latency deviation in ms.         */
} WF_CLIENT_LATENCY, * PWF_CLIENT_LATENCY;

/*==============================================================================
==   WFQuerySessionInformation - (WFVersionEx)
==   (Note: This flag returns Metaframe version in addition to the
==    NT version.)
==============================================================================*/

#ifdef UNICODE
#define WF_VERSIONEX_INFO  WF_VERSIONEX_INFOW
#define PWF_VERSIONEX_INFO PWF_VERSIONEX_INFOW
#else
#define WF_VERSIONEX_INFO  WF_VERSIONEX_INFOA
#define PWF_VERSIONEX_INFO PWF_VERSIONEX_INFOA
#endif

typedef struct _WF_VERSIONEX_INFOW
{
    DWORD dwVersionSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[128];
} WF_VERSIONEX_INFOW, * PWF_VERSIONEX_INFOW;

/*==============================================================================
==   WF_CONFIG_CLASS - WFQueryUserConfig/WFSetUserConfig
==============================================================================*/

typedef enum _WF_CONFIG_CLASS
{
    WFUserConfigInitialProgram,         // string returned/expected
    WFUserConfigWorkingDirectory,       // string returned/expected
    WFUserConfigfInheritInitialProgram  // DWORD returned/expected
} WF_CONFIG_CLASS;


/*==============================================================================
==   WF_EVENT - Event flags for WFWaitSystemEvent
==============================================================================*/

#define WF_EVENT_NONE         0x00000000 // return no event
#define WF_EVENT_CREATE       0x00000001 // new WinStation created
#define WF_EVENT_DELETE       0x00000002 // existing WinStation deleted
#define WF_EVENT_RENAME       0x00000004 // existing WinStation renamed
#define WF_EVENT_CONNECT      0x00000008 // WinStation connect to client
#define WF_EVENT_DISCONNECT   0x00000010 // WinStation logged on without client
#define WF_EVENT_LOGON        0x00000020 // user logged on to existing
                                         // WinStation
#define WF_EVENT_LOGOFF       0x00000040 // user logged off from existing
                                         // WinStation
#define WF_EVENT_STATECHANGE  0x00000080 // WinStation state change
#define WF_EVENT_LICENSE      0x00000100 // license state change
#define WF_EVENT_ALL          0x7fffffff // wait for all event types
#define WF_EVENT_FLUSH        0x80000000 // unblock all waiters

/*==============================================================================
==   WF_VIRTUAL_CLASS - WFVirtualChannelQuery
==============================================================================*/
typedef enum _WF_VIRTUAL_CLASS
{
    WFVirtualClientData  // Virtual channel client module data (C2H data)
} WF_VIRTUAL_CLASS;

/*==============================================================================
==   WF_DISCON - Flags for WFDisconnectSessionEx()
==============================================================================*/

#define WF_DISCON_WAIT              0x00000001
#define WF_DISCON_NO_NOTIFY_CLIENT  0x00000002

typedef struct _WF_USER_INFOW
{
    LPWSTR UserName;
    LPWSTR DomainName;
    LPWSTR ConnectionName;
} WF_USER_INFOW, * PWF_USER_INFOW;

typedef struct _WF_USER_INFOA
{
    LPSTR UserName;
    LPSTR DomainName;
    LPSTR ConnectionName;
} WF_USER_INFOA, * PWF_USER_INFOA;

typedef struct _WF_APP_INFOW
{
    LPWSTR InitialProgram;
    LPWSTR WorkingDirectory;
    LPWSTR ApplicationName;
} WF_APP_INFOW, * PWF_APP_INFOW;

typedef struct _WF_APP_INFOA
{
    LPSTR InitialProgram;
    LPSTR WorkingDirectory;
    LPSTR ApplicationName;
} WF_APP_INFOA, * PWF_APP_INFOA;

typedef struct _WF_CLIENT_INFOW
{
    LPWSTR            Name;
    LPWSTR            Directory;
    ULONG             BuildNumber;
    ULONG             ProductID;
    ULONG             HardwareID;
    WF_CLIENT_ADDRESS Address;
} WF_CLIENT_INFOW, *PWF_CLIENT_INFOW;

typedef struct _WF_CLIENT_INFOA
{
    LPSTR             Name;
    LPSTR             Directory;
    ULONG             BuildNumber;
    ULONG             ProductID;
    ULONG             HardwareID;
    WF_CLIENT_ADDRESS Address;
} WF_CLIENT_INFOA, *PWF_CLIENT_INFOA;

/*==============================================================================
==   WF_SESSION_TIME.  For WFSessionTime class.
==
==   The time values are all in FILETIME format.  Each time value is the number
==   of 100 nanosecond intervals from Jan. 1, 1601.  The time is GMT.
==============================================================================*/

typedef struct _WF_SESSION_TIME
{
    LARGE_INTEGER ConnectTime;
    LARGE_INTEGER DisconnectTime;
    LARGE_INTEGER LastInputTime;
    LARGE_INTEGER LogonTime;
    LARGE_INTEGER CurrentTime;
} WF_SESSION_TIME, *PWF_SESSION_TIME;

#ifdef UNICODE
#define WF_USER_INFO        WF_USER_INFOW
#define PWF_USER_INFO       PWF_USER_INFOW
#define WF_APP_INFO         WF_APP_INFOW
#define PWF_APP_INFO        PWF_APP_INFOW
#define WF_CLIENT_INFO      WF_CLIENT_INFOW
#define PWF_CLIENT_INFO     PWF_CLIENT_INFOW
#else
#define WF_USER_INFO        WF_USER_INFOA
#define PWF_USER_INFO       PWF_USER_INFOA
#define WF_APP_INFO         WF_APP_INFOA
#define PWF_APP_INFO        PWF_APP_INFOA
#define WF_CLIENT_INFO      WF_CLIENT_INFOA
#define PWF_CLIENT_INFO     PWF_CLIENT_INFOA
#endif

/*==============================================================================
==    Window Positioning SDK structures
==============================================================================*/

/*  
 *  Supported methods of notifying subscribers of window changes.
 *  Currently only support mail slots (WFMailslotResponse).
 *  Used in WF_RESPONSE_METHOD
 */
typedef enum _WF_RESPONSE_METHOD_TYPE 
{ 
    WFReservedResponse = 0, 
    WFMailslotResponse = 1
} WF_RESPONSE_METHOD_TYPE;

/* 
 *    Types of window update messages. Used in WF_WINDOW_DATA
 */
typedef enum _WF_WINDOW_MESSAGE_TYPE
{
    WFWindowUpdated = 0,
    WFWindowDeleted = 1
} WF_WINDOW_MESSAGE_TYPE;

/*
 *    Used in WF_RESPONSE_METHOD.
 *
 *    hSlot                [out] The SDK will populate this field with the mailslot 
 *                                handle the user can read from.
 *  WaitTime            [in]  The mailslot lReadTimeout parameter.
 *  pSecurityAttributes [in]  The mailslot lpSecurityAttributes parameter.
 */
typedef struct _WF_MAILSLOT_INFO
{
    HANDLE                    hSlot;                
    DWORD                    WaitTime;            
    LPSECURITY_ATTRIBUTES    pSecurityAttributes;
} WF_MAILSLOT_INFO;

/*
 *  Used by WFSubscribe.
 *    Contains the user's response information for window updates. 
 *    Is also populated in return with information from the SDK based on the 
 *  response method chosen.
 *
 *  type                [in]    Type of communication to be used
 *    params                [inout] Specific parameters based on the response method
 */
typedef struct _WF_RESPONSE_METHOD
{
    WF_RESPONSE_METHOD_TYPE Type;
    union PARAMS
    {
        WF_MAILSLOT_INFO MailSlot;    // Parameters needed for mailslots
    } Params;         
} WF_RESPONSE_METHOD, * PWF_RESPONSE_METHOD;

/*
 *    Used in WF_WINDOW_DATA to store visible region info
 */
typedef struct _WF_ENUMRECTS {
  ULONG  c;
  RECTL  arcl[1];
} WF_ENUMRECTS;

/*
 *    Mailslot message sent by SDK whenever a tracked window changes.
 *
 *    MessageType        Either WFWindowUpdated, or WFWindowDeleted. If deleted 
 *                        disregard the rest of the message.
 *    hWindow            The window this message refers to.
 *    RclClient        The client area rectangle of the window.
 *    Height            The window's height.
 *    Width            The window's width.
 *    iDComplexity    The complexity of the window's visible region:
 *                        DC_COMPLEX, DC_RECT, or DC_TRIVIAL.
 *    RclBounds        The bounding rectangle of the entire window.
 *    bMore            Flag indicating the visible region didn't fit in one 
 *                        message, and is being sent in additional messages.
 *    EnumRects        Clipping information. Contains the total number of 
 *                        rectangles in this message, and first rectangle.
 *    RclMoreClip        More clipping rectangles (if any). This must be the last
 *                        element in the structure, and follow EnumRects.
 */
typedef struct _WF_WINDOW_DATA
{
    WF_WINDOW_MESSAGE_TYPE    MessageType;     
    UINT                    hWindow;         
    RECTL                    RclClient;       
    ULONG                    Height;          
    ULONG                    Width;           
    BYTE                    iDComplexity;     
    RECTL                    RclBounds;       
    BOOL                    bMore;            
    WF_ENUMRECTS            EnumRects;        
    RECTL                    RclMoreClip[WF_NUM_REGION_RECTS];  
} WF_WINDOW_DATA, * PWF_WINDOW_DATA;

/*
 * WF_SMC_SERVER_DATA (see CtxSmcApi.h)
 */
#define WF_SMC_SERVER_DATA       Ctx_Smc_Server_Data
#define PWF_SMC_SERVER_DATA      pCtx_Smc_Server_Data

/*
 * WF_SMC_SERVER_VC_DATA (see CtxSmcApi.h)
 */
#define WF_SMC_SERVER_VC_DATA    Ctx_Smc_Server_VC_Data
#define PWF_SMC_SERVER_VC_DATA   pCtx_Smc_Server_VC_Data

/*
 * WF_SMC_SESSION_DATA (see CtxSmcApi.h)
 */
#define WF_SMC_SESSION_DATA      Ctx_Smc_Session_Data
#define PWF_SMC_SESSION_DATA     pCtx_Smc_Session_Data

/*
 * WF_SMC_SESSION_VC_DATA (see CtxSmcApi.h)
 */
#define WF_SMC_SESSION_VC_DATA   Ctx_Smc_Session_VC_Data
#define PWF_SMC_SESSION_VC_DATA  pCtx_Smc_Session_VC_Data

/*
 * WF_SMC_SESSION_VC_NAME (see CtxSmcApi.h)
 */
#define WF_SMC_SESSION_VC_NAME   Ctx_Smc_Session_VC_Name
#define PWF_SMC_SESSION_VC_NAME  pCtx_Smc_Session_VC_Name


/*==============================================================================
==   Citrix server public APIs
==============================================================================*/

BOOL WINAPI WFEnumerateServersW(IN  LPWSTR            pDomainName,
                                IN  DWORD             Reserved,
                                IN  DWORD             Version,
                                OUT PWF_SERVER_INFOW* ppServerInfo,
                                OUT DWORD*            pCount);

BOOL WINAPI WFEnumerateServersA(IN  LPSTR             pDomainName,
                                IN  DWORD             Reserved,
                                IN  DWORD             Version,
                                OUT PWF_SERVER_INFOA* ppServerInfo,
                                OUT DWORD*            pCount);

#ifdef UNICODE
#define WFEnumerateServers WFEnumerateServersW
#else
#define WFEnumerateServers WFEnumerateServersA
#endif

/*----------------------------------------------------------------------------*/

HANDLE WINAPI WFOpenServerW(IN LPWSTR pServerName);

HANDLE WINAPI WFOpenServerA(IN LPSTR pServerName);

#ifdef UNICODE
#define WFOpenServer WFOpenServerW
#else
#define WFOpenServer WFOpenServerA
#endif

/*----------------------------------------------------------------------------*/

VOID WINAPI WFCloseServer(IN HANDLE hServer);

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFEnumerateSessionsW(IN  HANDLE             hServer,
                                 IN  DWORD              Reserved,
                                 IN  DWORD              Version,
                                 OUT PWF_SESSION_INFOW* ppSessionInfo,
                                 OUT DWORD*             pCount);

BOOL WINAPI WFEnumerateSessionsA(IN  HANDLE             hServer,
                                 IN  DWORD              Reserved,
                                 IN  DWORD              Version,
                                 OUT PWF_SESSION_INFOA* ppSessionInfo,
                                 OUT DWORD*             pCount);

#ifdef UNICODE
#define WFEnumerateSessions WFEnumerateSessionsW
#else
#define WFEnumerateSessions WFEnumerateSessionsA
#endif

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFEnumerateProcessesW(IN  HANDLE             hServer,
                                  IN  DWORD              Reserved,
                                  IN  DWORD              Version,
                                  OUT PWF_PROCESS_INFOW* ppProcessInfo,
                                  OUT DWORD*             pCount);

BOOL WINAPI WFEnumerateProcessesA(IN  HANDLE             hServer,
                                  IN  DWORD              Reserved,
                                  IN  DWORD              Version,
                                  OUT PWF_PROCESS_INFOA* ppProcessInfo,
                                  OUT DWORD*             pCount);

#ifdef UNICODE
#define WFEnumerateProcesses WFEnumerateProcessesW
#else
#define WFEnumerateProcesses WFEnumerateProcessesA
#endif

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFEnumerateProcessesExW(IN  HANDLE               hServer,
                                    IN  DWORD                Reserved,
                                    IN  DWORD                Version,
                                    OUT PWF_PROCESS_INFOEXW* pProcessInfo,
                                    OUT DWORD*               pCount);

BOOL WINAPI WFEnumerateProcessesExA(IN  HANDLE               hServer,
                                    IN  DWORD                Reserved,
                                    IN  DWORD                Version,
                                    OUT PWF_PROCESS_INFOEXA* pProcessInfo,
                                    OUT DWORD*               pCount);

#ifdef UNICODE
#define WFEnumerateProcessesEx WFEnumerateProcessesExW
#else
#define WFEnumerateProcessesEx WFEnumerateProcessesExA
#endif

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFTerminateProcess(IN HANDLE hServer,
                               IN DWORD  ProcessId,
                               IN DWORD  ExitCode);

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFQuerySessionInformationW(IN  HANDLE        hServer,
                                       IN  DWORD         SessionId,
                                       IN  WF_INFO_CLASS WFInfoClass,
                                       OUT LPWSTR*       ppBuffer,
                                       OUT DWORD*        pBytesReturned);

BOOL WINAPI WFQuerySessionInformationA(IN  HANDLE        hServer,
                                       IN  DWORD         SessionId,
                                       IN  WF_INFO_CLASS WFInfoClass,
                                       OUT LPSTR*        ppBuffer,
                                       OUT DWORD*        pBytesReturned);

#ifdef UNICODE
#define WFQuerySessionInformation WFQuerySessionInformationW
#else
#define WFQuerySessionInformation WFQuerySessionInformationA
#endif

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFQueryUserConfigW(IN  LPWSTR          pServerName,
                               IN  LPWSTR          pUserName,
                               IN  WF_CONFIG_CLASS WFConfigClass,
                               OUT LPWSTR*         ppBuffer,
                               OUT DWORD*          pBytesReturned);

BOOL WINAPI WFQueryUserConfigA(IN  LPSTR           pServerName,
                               IN  LPSTR           pUserName,
                               IN  WF_CONFIG_CLASS WFConfigClass,
                               OUT LPSTR*          ppBuffer,
                               OUT DWORD*          pBytesReturned);

#ifdef UNICODE
#define WFQueryUserConfig WFQueryUserConfigW
#else
#define WFQueryUserConfig WFQueryUserConfigA
#endif

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFSetUserConfigW(IN LPWSTR          pServerName,
                             IN LPWSTR          pUserName,
                             IN WF_CONFIG_CLASS WFConfigClass,
                             IN LPWSTR          pBuffer,
                             IN DWORD           DataLength);

BOOL WINAPI WFSetUserConfigA(IN LPSTR           pServerName,
                             IN LPSTR           pUserName,
                             IN WF_CONFIG_CLASS WFConfigClass,
                             IN LPSTR           pBuffer,
                             IN DWORD           DataLength);

#ifdef UNICODE
#define WFSetUserConfig WFSetUserConfigW
#else
#define WFSetUserConfig WFSetUserConfigA
#endif

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFSendMessageW(IN  HANDLE hServer,
                           IN  DWORD  SessionId,
                           IN  LPWSTR pTitle,
                           IN  DWORD  TitleLength,
                           IN  LPWSTR pMessage,
                           IN  DWORD  MessageLength,
                           IN  DWORD  Style,
                           IN  DWORD  Timeout,
                           OUT DWORD* pResponse,
                           IN  BOOL   bWait);

BOOL WINAPI WFSendMessageA(IN  HANDLE hServer,
                           IN  DWORD  SessionId,
                           IN  LPSTR  pTitle,
                           IN  DWORD  TitleLength,
                           IN  LPSTR  pMessage,
                           IN  DWORD  MessageLength,
                           IN  DWORD  Style,
                           IN  DWORD  Timeout,
                           OUT DWORD* pResponse,
                           IN  BOOL   bWait);

#ifdef UNICODE
#define WFSendMessage WFSendMessageW
#else
#define WFSendMessage WFSendMessageA
#endif

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFDisconnectSession(IN HANDLE hServer,
                                IN DWORD  SessionId,
                                IN BOOL   bWait);

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFDisconnectSessionEx(IN HANDLE hServer,
                                  IN DWORD  SessionId,
                                  IN ULONG  Flags,
                                  IN PVOID  pBuffer,
                                  IN ULONG  BufferLength);

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFLogoffSession(IN HANDLE hServer,
                            IN DWORD SessionId,
                            IN BOOL bWait);

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFShutdownSystem(IN HANDLE hServer,
                             IN DWORD ShutdownFlag);

/*----------------------------------------------------------------------------*/

BOOL WINAPI WFWaitSystemEvent(IN  HANDLE hServer,
                              IN  DWORD  EventMask,
                              OUT DWORD* pEventFlags);

/*----------------------------------------------------------------------------*/

HANDLE WINAPI WFHandleToPVCHandle( IN HANDLE hChannel,
                                    IN HANDLE hServer,
                                   IN DWORD  SessionId,
                                   IN LPSTR  pVirtualName); /* ascii name */


HANDLE WINAPI WFVirtualChannelOpen(IN HANDLE hServer,
                                   IN DWORD  SessionId,
                                   IN LPSTR  pVirtualName); /* ascii name */

BOOL WINAPI WFVirtualChannelCleanup(IN HANDLE hChannelHandle);

BOOL WINAPI WFVirtualChannelClose(IN HANDLE hChannelHandle);

BOOL WINAPI WFVirtualChannelRead(IN  HANDLE hChannelHandle,
                                 IN  ULONG  TimeOut,
                                 OUT PCHAR  Buffer,
                                 IN  ULONG  BufferSize,
                                 OUT PULONG pBytesRead);

BOOL WINAPI WFVirtualChannelReadEx(IN  HANDLE hChannelHandle,
                                   IN  HANDLE hCancelEvent,
                                   OUT PCHAR  Buffer,
                                   IN  ULONG  BufferSize,
                                   OUT PULONG pBytesRead);

BOOL WINAPI WFVirtualChannelWrite(IN  HANDLE hChannelHandle,
                                  IN  PCHAR  Buffer,
                                  IN  ULONG  Length,
                                  OUT PULONG pBytesWritten);

BOOL WINAPI WFVirtualChannelPurgeInput(IN HANDLE hChannelHandle);

BOOL WINAPI WFVirtualChannelPurgeOutput(IN HANDLE hChannelHandle);

BOOL WINAPI WFVirtualChannelQuery(IN  HANDLE           hChannelHandle,
                                  IN  WF_VIRTUAL_CLASS VirtualClass,
                                  OUT PVOID*           ppBuffer,
                                  OUT DWORD*           pBytesReturned);

BOOL WINAPI WFVirtualChannelSupportsShadow(IN HANDLE hChannelHandle);

/*----------------------------------------------------------------------------*/

WF_PROTOCOL WINAPI WFGetActiveProtocol( IN DWORD   SessionId ); 

/*----------------------------------------------------------------------------*/

VOID WINAPI WFFreeMemory(IN PVOID pMemory);

BOOL WINAPI WFShadowSessionA(IN LPSTR pServerName,
                             IN ULONG SessionID,
                             IN ULONG HotKey,
                             IN ULONG HKModifier);

BOOL WINAPI WFShadowSessionW(IN LPWSTR pServerName,
                             IN ULONG  SessionID,
                             IN ULONG  HotKey,
                             IN ULONG  HKModifier);

#ifdef UNICODE
#define WFShadowSession WFShadowSessionW
#else
#define WFShadowSession WFShadowSessionA
#endif

/*----------------------------------------------------------------------------*/

VOID WINAPI WFTrackingInit();

VOID WINAPI WFTrackingTerminate();

BOOL WINAPI WFSubscribe(OUT        UINT*                pUserId, 
                        IN OUT    PWF_RESPONSE_METHOD pCommMethod);

BOOL WINAPI WFUnsubscribe(IN UINT UserId);

BOOL WINAPI WFStartTrackingWindow(IN UINT Id,
                                  IN UINT hWnd);

BOOL WINAPI WFStopTrackingWindow(IN UINT Id,
                                 IN UINT hWnd);

/*----------------------------------------------------------------------------*/

DWORD  WINAPI WFSmcGetVersionSupported(IN  HANDLE  hServer,
                                       OUT UINT32* pVersionSupported,
                                       OUT UINT32* pFeaturesSupported);

DWORD  WINAPI WFSmcGetServerData(IN       HANDLE                    hServer,
                                 IN       UINT32                    cbData, 
                                 OUT      PWF_SMC_SERVER_DATA       pData,
                                 IN       UINT32                    cbServerVCData, 
                                 OUT      PWF_SMC_SERVER_VC_DATA    pServerVCData,
                                 IN OUT   UINT32*                   pcVCs);

DWORD WINAPI WFSmcOpenSessionHandle(IN  HANDLE  hServer,
                                    IN  UINT32  SessionId, 
                                    OUT PHANDLE phSession);

DWORD WINAPI WFSmcCloseSessionHandle(IN HANDLE hServer,
                                     IN HANDLE hSession);

DWORD  WINAPI WFSmcGetSessionVirtualChannelNames(IN       HANDLE                    hServer,
                                                 IN       HANDLE                    hSession,
                                                 IN       UINT32                    cbSessionVCNames, 
                                                 OUT      PWF_SMC_SESSION_VC_NAME   pSessionVCNames, 
                                                 IN OUT   UINT32*                   pcSessionVCNames);

DWORD  WINAPI WFSmcGetSessionData(IN      HANDLE                    hServer,
                                  IN      HANDLE                    hSession, 
                                  IN      UINT32                    cbData, 
                                  OUT     PWF_SMC_SESSION_DATA      pData, 
                                  IN      UINT32                    cbVCData, 
                                  OUT     PWF_SMC_SESSION_VC_DATA   pVCData, 
                                  IN OUT  UINT32*                   pcVC);

DWORD  WINAPI WFSmcGetSessionLatencyData(IN   HANDLE  hServer,
                                         IN   HANDLE  hSession, 
                                         OUT  UINT32* pLastLatency, 
                                         OUT  UINT32* pAverageLatency, 
                                         OUT  UINT32* pRoundTripDeviation, 
                                         OUT  UINT32* pTickCountLastUpdateTime);

DWORD  WINAPI WFSmcGetSessionBandwidthLimits(IN   HANDLE  hServer,
                                             IN   HANDLE  hSession,
                                             IN   UINT32  cEntries, 
                                             IN   UINT32* pVCIDs, 
                                             OUT  UINT32* pVCLimits);

DWORD  WINAPI WFSmcSetSessionBandwidthLimits(IN HANDLE  hServer,
                                             IN HANDLE  hSession,
                                             IN UINT32  cEntries, 
                                             IN UINT32* pVCIDs, 
                                             IN UINT32* pVCLimits);

DWORD  WINAPI WFSmcGetSessionPriorities(IN   HANDLE  hServer,
                                        IN   HANDLE  hSession,
                                        IN   UINT32  cEntries, 
                                        IN   UINT32* pVCIDs, 
                                        OUT  UINT32* pVCPriorities);

DWORD  WINAPI WFSmcSetSessionPriorities(IN HANDLE  hServer,
                                        IN HANDLE  hSession, 
                                        IN UINT32  cEntries, 
                                        IN UINT32* pVCIDs, 
                                        IN UINT32* pVCPriorities);

//         caller is responsible for freeing memory wih LocalFree()
DWORD WINAPI WFGetCtxSessionKeyList(HANDLE hServer, WCHAR **SessionKeys, DWORD *KeyCount, DWORD *dwKeyBufferLen, DWORD *KeyStrLen);

DWORD  WINAPI WFSmcGetSessionDataV2(IN      HANDLE                       hServer,
                                    IN      HANDLE                       hSession, 
                                    IN      UINT32                       cbData, 
                                    OUT     pCtx_Smc_Session_Data_V2     pData, 
                                    IN      UINT32                       cbVCData, 
                                    OUT     pCtx_Smc_Session_VC_Data_V2  pVCData, 
                                    IN OUT  UINT32*                      pcVC);


BOOL WINAPI WFIsThisDoubleHopSession(IN HANDLE hServer,
	                                 IN DWORD SessionId);

BOOL WINAPI WFWaitEndPointClientEventInDoubleHop(IN HANDLE hServer,
	                                             IN DWORD EventMask,
	                                             OUT DWORD * pEventFlags);
BOOL WINAPI WFEndPointClientDataInDoubleHopW(IN HANDLE hServer,
	                                         IN DWORD SessionId,
	                                         IN PWF_CLIENT_INFOW* ppClientInfo,
	                                         DWORD* DataSize);

BOOL WINAPI WFEndPointClientDataInDoubleHopA(IN HANDLE hServer,
											 IN DWORD SessionId,
											 IN PWF_CLIENT_INFOA* ppClientInfo,
											 DWORD* DataSize
											);

#ifdef UNICODE
#define WFEndPointClientDataInDoubleHop WFEndPointClientDataInDoubleHopW
#else
#define WFEndPointClientDataInDoubleHop WFEndPointClientDataInDoubleHopA
#endif
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif  /* !_INC_WFAPI */

