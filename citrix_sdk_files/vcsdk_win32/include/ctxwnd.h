/******************************************************************************
 *
 *  ctxwnd.h
 *
 *  Window Object Interface
 *
 *  Copyright 1999-2015 Citrix Systems, Inc. 
 *
 ******************************************************************************/

#ifndef __CTXWND_H_
#define __CTXWND_H_

#pragma once
#include <ctxgraph.h> // reference to HGCURSOR.  Needed for \src\ ONLY!
#include <clpnp.h>

//=============================================================================
//   Definitions
//=============================================================================

#define IMEFILENAME_LENGTH    32
#define	WND_EVTID_MOUSEINFO	   1				// mouse	data delivered (WND_MOUSEINFO)
#define	WND_EVTID_SCANCODEINFO 2				// keyboard scancodes delivered (WND_SCANCODEINFO)
#define	WND_EVTID_CHARCODEINFO 3				// keyboard charcodes delivered (WND_CHARCODEINFO)
#define	WND_EVTID_UNICODEINFO  4				// keyboard unicode delivered (WND_UNICODEINFO)
#define	WND_EVTID_FOCUSINFO	   5				// window focus has changed (WND_FOCUSINFO)
#define	WND_EVTID_PAINTINFO	   6				// window must be repainted (WND_PAINTINFO)
#define	WND_EVTID_BOUNDSINFO   7				// window	has	been moved/resized (WND_BOUNDSINFO)
#define	WND_EVTID_UNICODEINSERT	8				// Direct UNICODE char insertion	(WND_UNICODECHARINFO)
#define WND_EVTID_CHANGE_SEAMLESS_MODE 9      /* Change seamless mode to paused or resume */
#define WND_EVTID_BRING_TO_FRONT 10     /* Bring this sessions windows to the front of the z-order */        
#define WND_EVTID_ADD_NO_REFLECT_WND 11 /* Add this window as a non-reflected reverse seamless window */
#define WND_EVTID_REMOVE_ALL_SESSION_NO_REFLECT_WND 12 /* Remove all non-reflected windows for a session for a given application */
#define WND_EVTID_MOUSEINFO_RAW    14           // mouse	data delivered (WND_MOUSEINFO)
#define WND_EVTID_SHOW_TOASTER		21			// Toaster Notification for RSManager 

#define WND_FOCUSINFO_GAINING 1
#define WND_FOCUSINFO_GAINED  2
#define WND_FOCUSINFO_LOSING  3
#define WND_FOCUSINFO_LOST    4
#ifndef WND_KBDSTATE_CAPSLOCK // these could be defined by \srcvs includes
#define	WND_KBDSTATE_CAPSLOCK	(0x00000001)	//	Flags used by the Wnd_get/setKeyboardState function.
#define	WND_KBDSTATE_NUMLOCK	(0x00000002)
#define	WND_KBDSTATE_SCROLLLOCK	(0x00000004)
#endif
#define WND_MOUSEDATA_MOVED		0x01			// Mouse state flags
#define WND_MOUSEDATA_B1DOWN	0x02
#define WND_MOUSEDATA_B1UP		0x04
#define WND_MOUSEDATA_B2DOWN	0x08
#define WND_MOUSEDATA_B2UP		0x10
#define WND_MOUSEDATA_B3DOWN	0x20
#define WND_MOUSEDATA_B3UP		0x40
#define WND_MOUSEDATA_DBLCLK	0x80
#define WND_MOUSEDATA_B4DOWN	0x02
#define WND_MOUSEDATA_B4UP		0x04
#define WND_MOUSEDATA_B5DOWN	0x08
#define WND_MOUSEDATA_B5UP		0x10
#define WND_MOUSEDATA_WHEEL		0x20

#define WND_MOUSEDATA_NORMAL	0				// mouse data type
#define WND_MOUSEDATA_EXTRA		1
#define WND_MOUSEDATA_RAW       2               //ui type for raw mouse data

//Mouse flags for Raw mode
#define WND_RAW_MOUSE_B1DOWN    0x0001
#define WND_RAW_MOUSE_B1UP      0x0002
#define WND_RAW_MOUSE_B2DOWN    0x0004
#define WND_RAW_MOUSE_B2UP      0x0008
#define WND_RAW_MOUSE_B3DOWN    0x0010
#define WND_RAW_MOUSE_B3UP      0x0020

#define WND_RAW_MOUSE_B4DOWN    0x0040
#define WND_RAW_MOUSE_B4UP      0x0080
#define WND_RAW_MOUSE_B5DOWN    0x0100
#define WND_RAW_MOUSE_B5UP      0x0200
#define WND_RAW_MOUSE_WHEEL     0x0400
#define WND_RAW_MOUSE_HWHEEL    0x0800

#define WND_RAW_MOUSE_DBLCLK    0x1000 //Synthesized
#define WND_RAW_MOUSE_ABSOLUTE  0x2000
#define WND_RAW_MOUSE_RELATIVE  0x4000

#define	WND_STATUS_SUCCESS			   0
#define	WND_STATUS_BADHANDLE		   1		// abstract window	handle is wrong
#define	WND_STATUS_BADPARAM			   2		// parameters to a	function is	bad
#define	WND_STATUS_NOMEMORY			   3		// failure	to allocate	memory
#define	WND_STATUS_INSUFFICIENT_BUFFER 4		// insufficient buffer	size
#define	WND_STATUS_ERROR			   5		// miscellaneous error	message
#define WND_UNICODECHAR_KEYUP   0x01			// unicode keyboard flags
#define WND_UNICODECHAR_SPECIAL 0x02
#define WND_UNICODECHAR_DIRECT  0x04

//=============================================================================
//   Enums
//=============================================================================
#ifndef __PAL_COMMON_BASE_H__ //TODO: Already merged elsewhere
// WND_TYPE
//
// Type of window
//
typedef	enum _WND_TYPE
{
	WND_TYPE_EMBEDDED,				// a child window to be placed within a parent window
	WND_TYPE_OTHER					// an unknown type (used for natively created windows)
} WND_TYPE;

//=============================================================================
//   Typedefs
//=============================================================================

// WND_EVENTINFO
//
// Window event structure delivered using pfnDeliver function.
//
typedef struct WND_EVENTINFO
{
	HND	  hSource;					// the source window
	UINT8 uiEventID;				// indicates the type of window event
} WND_EVENTINFO, *PWND_EVENTINFO;

// WND_SCANCODEINFO
//
// Structure representing an interpreted keyboard character code w/shift state & repeat count
//
typedef struct WND_CHARCODEINFO
{
	WND_EVENTINFO info;
	UINT16 cCharCodes;
	LPBYTE pCharCodes;
	UINT16 uiRepeat;
	UINT16 uiShiftState;
} WND_CHARCODEINFO, *PWND_CHARCODEINFO;

// WND_FOCUSINFO
//
// This event is delivered when the focus state of a window is changing.
//
typedef struct _WND_FOCUSINFO
{
	WND_EVENTINFO info;
	UINT32 uiState;
} WND_FOCUSINFO, *PWND_FOCUSINFO;

#ifndef SEAMLESS_MODE_RESUME
#define SEAMLESS_MODE_RESUME        ((UINT32)1)
#define SEAMLESS_MODE_PAUSE         ((UINT32)2)
#define SEAMLESS_MODE_TOGGLE        ((UINT32)3)
#endif //#ifndef SEAMLESS_MODE_RESUME

typedef struct WND_CHANGE_SEAMLESS_INFO
{
    WND_EVENTINFO   info;
    UINT32          uiMode;
}WND_CHANGE_SEAMLESS_INFO, *PWND_CHANGE_SEAMLESS_INFO;

typedef struct WND_BRING_TO_FRONT
{
    WND_EVENTINFO   info;
}WND_BRING_TO_FRONT, *PWND_BRING_TO_FRONT;

typedef struct WND_SHOW_TOASTER
{
    WND_EVENTINFO   info;
}WND_SHOW_TOASTER;

typedef struct WND_ADD_NO_REFLECT_WND
{
    WND_EVENTINFO   info;
    HND             hWindowToAdd;   //Window to add as a non-reflected window
}WND_ADD_NO_REFLECT_WND, *PWND_ADD_NO_REFLECT_WND;

typedef struct WND_REMOVE_NO_REFLECT_WND
{
    WND_EVENTINFO   info;
    UINT32          uiPID;   //PID associated with the windows to be removed
}WND_REMOVE_NO_REFLECT_WND, *PWND_REMOVE_NO_REFLECT_WND;
#ifndef REMOVE_ALL_NO_REFLECT_WND
#define REMOVE_ALL_NO_REFLECT_WND   (0xffffffff)
#endif //#ifndef REMOVE_ALL_NO_REFLECT_WND


// WND_INPUTPREFERENCES
//
// Structure to store various input preferences
//
typedef struct _WND_INPUTPREFERENCES
{
	UINT16  uiKeyboardDelay;
	UINT16  uiKeyboardSpeed;
	BOOL    fSwapButtons;
	UINT16  uiHorizSpeed;
	UINT16  uiVertSpeed;
	UINT16  uiDblSpeedThreshold;
} WND_INPUTPREFERENCES, *PWND_INPUTPREFERENCES;

// WND_KEYBOARDINFO
//
// General information about the user's keyboard
//
typedef struct _WND_KEYBOARDINFO
{
	UINT32 uiKeyboardLayout;
	UINT16 uiKeyboardType;				// Keyboard	type (PC/XT, PC/AT,	Japanese...)
	UINT8  uiKeyboardSubType;			// Keyboard	subtype	(US	101, JPN 106)
	UINT8  uiKeyboardFunctionKey;		// Number of function keys
	UINT16 uiKeyboardSubtype2;
	CHAR   imeFileName[IMEFILENAME_LENGTH+1];	// IME file	name
	BOOL   fWinKeyFlag;					// Flag	to decide if Client	can	send left and right	windows	keys through
} WND_KEYBOARDINFO, *PWND_KEYBOARDINFO;

// WND_MOUSEDATA
//
// Structure representing one mouse event
//
typedef struct _WND_MOUSEDATA
{
	UINT16 X;
	UINT16 Y;
	UINT16  cMouState;
	UINT8  uiType;
	UINT16 uiExtraInfo;
} WND_MOUSEDATA, *PWND_MOUSEDATA;

// WND_MOUSEINFO
//
// Structure representing a set of mouse events
//
typedef struct _WND_MOUSEINFO
{
	WND_EVENTINFO info;
	UINT16 cMouseData;				// the number of WND_MOUSEDATA structures
	PWND_MOUSEDATA pMouseData;      // an array of WND_MOUSEDATA structures
} WND_MOUSEINFO, *PWND_MOUSEINFO;

// WND_PAINTINFO
//
// Structure for painting...passes through basic WND_EVENTINFO only
//
typedef struct _WND_PAINTINFO
{
	WND_EVENTINFO info;
} WND_PAINTINFO, *PWND_PAINTINFO;

// WND_SCANCODEINFO
//
// Structure representing a keyboard scan code w/shift state & repeat count
//
typedef struct _WND_SCANCODEINFO
{    
	WND_EVENTINFO info;
	UINT16 cScanCodes;
	PUCHAR  pScanCodes;
	UINT16 uiRepeat;
	UINT16 uiShiftState;
} WND_SCANCODEINFO, *PWND_SCANCODEINFO;

// WND_UNICODECHAR
//
// Structure representing a single unicode keyboard character
//
typedef struct _WND_UNICODECHAR
{
	UINT8  uiFlags;					// the type of char value
	UINT16 uiValue;					// Unicode or special key value
} WND_UNICODECHAR, *PWND_UNICODECHAR;

// WND_UNICODECHARINFO
//
// Structure representing an interpreted unicode character w/optional repeat count
//	for direct unicode insertion
//
typedef struct _WND_UNICODECHARINFO
{
	WND_EVENTINFO   info;     
	WND_UNICODECHAR uc;				// Unicode character
	UINT16          uiRepeat;
} WND_UNICODECHARINFO, *PWND_UNICODECHARINFO;

// WND_UNICODEINFO
//
// Structure representing a unicode keyboard scan code w/shift state & repeat count
//
typedef struct _WND_UNICODEINFO
{
	WND_EVENTINFO info;
	UINT16 cUnicodeCodes;
	WND_UNICODECHAR	UnicodeCodes;		  
	UINT16 uiRepeat;
	UINT16 uiShiftState;
} WND_UNICODEINFO, *PWND_UNICODEINFO;
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef VPSTATUS (_PVPAPI PFNWNDCREATE)(WND_TYPE, PVPRECT, HND, PVOID, PFNDELIVER,HND,PHND);
typedef VPSTATUS (_PVPAPI PFNWNDSETUSERDATA)(HND,PVOID);
typedef VPSTATUS (_PVPAPI PFNWNDGETUSERDATA)(HND,PPVOID);
typedef VPSTATUS (_PVPAPI PFNWNDSETPARENT)(HND, HND);
typedef VPSTATUS (_PVPAPI PFNWNDSETVISIBLE)(HND, BOOL);
typedef VPSTATUS (_PVPAPI PFNWNDSETSIZE)(HND, PVPSIZE);
typedef VPSTATUS (_PVPAPI PFNWNDGETSIZE)(HND, PVPSIZE);
typedef VPSTATUS (_PVPAPI PFNWNDSETPOS)(HND, PVPPOINT);
typedef VPSTATUS (_PVPAPI PFNWNDREPAINTRECT)(HND, PVPRECT);
typedef VPSTATUS (_PVPAPI PFNWNDGETREPAINTRECTS)(HND,PVPRECT,INT16,PINT16);
typedef VPSTATUS (_PVPAPI PFNWNDGETOBSCUREDRECTS)(HND,PVPRECT,PVPRECT,INT16,PINT16);
typedef VPSTATUS (_PVPAPI PFNWNDSETINPUTPREFERENCES)(PWND_INPUTPREFERENCES);
typedef VPSTATUS (_PVPAPI PFNWNDGETKEYBOARDINFO)(UINT32,UINT32,CHAR*,PWND_KEYBOARDINFO);
typedef VPSTATUS (_PVPAPI PFNWNDGETKEYBOARDSTATE)(PUINT32);
typedef VPSTATUS (_PVPAPI PFNWNDSETKEYBOARDSTATE)(UINT32);
typedef VPSTATUS (_PVPAPI PFNWNDSETCURSOR)(HND, HGCURSOR);
typedef VPSTATUS (_PVPAPI PFNWNDSETMOUSEPOS)(INT32, INT32, HND);
typedef VPSTATUS (_PVPAPI PFNWNDREQUESTFOCUS)(HND);
typedef VPSTATUS (_PVPAPI PFNWNDDESTROY)(PHND);
typedef VPSTATUS (_PVPAPI PFNWNDEJECTPNP)(CTXPNP_HND32);
typedef VPSTATUS (_PVPAPI PFNWNDREGISTERDEVICE)(PCTXPNP_ELEMENT_HEADER, PFN_CLPNP_DEVICECHANGECALLBACK);
typedef VPSTATUS (_PVPAPI PFNWNDUNREGISTERDEVICE)(
#if defined(WIN32) 
        HANDLE,
#endif
        PFN_CLPNP_DEVICECHANGECALLBACK);

typedef struct _WNDCALLTABLE
{
    PFNWNDCREATE              pfnWndCreate;
    PFNWNDSETUSERDATA         pfnWndSetUserData;
    PFNWNDGETUSERDATA         pfnWndGetUserData;
    PFNWNDSETPARENT           pfnWndSetParent;
    PFNWNDSETVISIBLE          pfnWndSetVisible;
    PFNWNDSETSIZE             pfnWndSetSize;
    PFNWNDGETSIZE             pfnWndGetSize;
    PFNWNDSETPOS              pfnWndSetPos;
    PFNWNDREPAINTRECT         pfnWndRepaintRect;
    PFNWNDGETREPAINTRECTS     pfnGetRepaintRects;
    PFNWNDGETOBSCUREDRECTS    pfnGetObscuredRects;
    PFNWNDSETINPUTPREFERENCES pfnWndSetInputPreferences;
    PFNWNDGETKEYBOARDINFO     pfnWndGetKeyboardInfo;
    PFNWNDGETKEYBOARDSTATE    pfnWndGetKeyboardState;
    PFNWNDSETKEYBOARDSTATE    pfnWndSetKeyboardState;
    PFNWNDSETCURSOR           pfnWndSetCursor;
    PFNWNDSETMOUSEPOS         pfnWndSetMousePos;
    PFNWNDREQUESTFOCUS        pfnWndRequestFocus;
    PFNWNDDESTROY             pfnWndDestroy;
    PFNWNDEJECTPNP            pfnWndEjectPNP;
    PFNWNDREGISTERDEVICE      pfnWndRegisterDeviceChange;
    PFNWNDUNREGISTERDEVICE    pfnWndUnregisterDeviceChange;
} WNDCALLTABLE, FAR * PWNDCALLTABLE, FAR * FAR * PPWNDCALLTABLE;

#define WND_STRING        "CTXWND"

VPSTATUS _VPAPI Wnd_Load(PPWNDCALLTABLE ppWndCallTable);
VPSTATUS _VPAPI Wnd_Unload();

#ifdef  USE_WND_CALL_TABLE

#define WND_GLOBAL_INIT   PWNDCALLTABLE g_pWndCallTable = NULL
#define WND_EXTERN_INIT   extern PWNDCALLTABLE g_pWndCallTable 

WND_EXTERN_INIT;

#define Wnd_create               (*g_pWndCallTable->pfnWndCreate)
#define Wnd_setUserData          (*g_pWndCallTable->pfnWndSetUserData)
#define Wnd_getUserData          (*g_pWndCallTable->pfnWndGetUserData)
#define Wnd_setParent            (*g_pWndCallTable->pfnWndSetParent)
#define Wnd_setVisible           (*g_pWndCallTable->pfnWndSetVisible)
#define Wnd_setSize              (*g_pWndCallTable->pfnWndSetSize)
#define Wnd_getSize              (*g_pWndCallTable->pfnWndGetSize)
#define Wnd_setPos               (*g_pWndCallTable->pfnWndSetPos)
#define Wnd_repaintRect          (*g_pWndCallTable->pfnWndRepaintRect)
#define Wnd_getRepaintRects      (*g_pWndCallTable->pfnGetRepaintRects)
#define Wnd_getObscuredRects     (*g_pWndCallTable->pfnGetObscuredRects)
#define Wnd_setInputPreferences  (*g_pWndCallTable->pfnWndSetInputPreferences)
#define Wnd_getKeyboardInfo      (*g_pWndCallTable->pfnWndGetKeyboardInfo)
#define Wnd_getKeyboardState     (*g_pWndCallTable->pfnWndGetKeyboardState)
#define Wnd_setKeyboardState     (*g_pWndCallTable->pfnWndSetKeyboardState)
#define Wnd_setCursor            (*g_pWndCallTable->pfnWndSetCursor)
#define Wnd_setMousePos          (*g_pWndCallTable->pfnWndSetMousePos)
#define Wnd_requestFocus         (*g_pWndCallTable->pfnWndRequestFocus)
#define Wnd_destroy              (*g_pWndCallTable->pfnWndDestroy)
#define Wnd_EjectPNP             (*g_pWndCallTable->pfnWndEjectPNP)
#define Wnd_RegisterDeviceChange (*g_pWndCallTable->pfnWndRegisterDeviceChange)
#define Wnd_UnregisterDeviceChange (*g_pWndCallTable->pfnWndUnregisterDeviceChange)

#define WndRunTimeInit( libmgr, pver ) \
    ((*(libmgr->pLMgrUse))( WND_STRING, pver, (PPLMGRTABLE)&g_pWndCallTable ))
#define WndRuntimeRelease( libmgr ) \
    ((*(libmgr->pLMgrRelease))( (PLMGRTABLE)g_pWndCallTable ))

#else /* !USE_WND_CALL_TABLE */

#define WND_GLOBAL_INIT  
#define WND_EXTERN_INIT   

#define WndRunTimeInit( libmgr, pver )
#define WndRuntimeRelease( libmgr ) 

/*******************************************************************************
 *
 *  Exported Functions
 *
 *    Wnd_create
 *    Wnd_setUserData
 *    Wnd_getUserData
 *    Wnd_setParent
 *    Wnd_setVisible
 *    Wnd_setSize
 *    Wnd_getSize
 *    Wnd_setPos
 *    Wnd_repaintRect 
 *    Wnd_getRepaintRects
 *    Wnd_getObscuredRects
 *    Wnd_setCursor
 *    Wnd_setMousePos
 *    Wnd_requestFocus
 *    Wnd_destroy
 *    Wnd_EjectPNP
 *    Wnd_registerDeviceChangeCallback
 *    Wnd_unregisterDeviceChangeCallback
 *
 ******************************************************************************/

/*******************************************************************************
   Wnd_registerDeviceChangeCallback

      Registers a device-change-notification callback for a specific device type.

      Registered call-backs are triggered when PnP event notifications from the
      native OS occur, and also, immediately following registration, for device
      instances that are already present on the system (fake or power-up PnP
      events).

      The type of device is specified as a generic Citrix PnP Device element.
      See ctxpnp.h for a list of supported Device Elements.

      Example: Memory layout for an Interface Device Element:

      1. CTXPNP_ELEMENT_HEADER
         - The ByteCount field is the total number of bytes in the element and
           its associated data, if any, including the ByteCount field itself.
         - The Type field is CTXPNP_TYPE_DEVICE_INTERFACE.
      2. CTXPNP_DEVICE_INTERFACE
         - DeviceInterfaceClass is the GUID of the device interface class to
           which the device instance represented by this PnP event belongs.
           In the first version of the interface DeviceInterfaceClass is
           required.
         - VendorID, and ProductID are optional in the first version of the
           interface, zero if not used.
         - oDeviceName is ignored.
         - Reserved[2] must be zero.
         - DeviceNameByteCount is ignored.

      If pDeviceElement or pfn_DeviceChangeCallback are NULL, an error is
      returned.

      The same callback can be registered for multiple device types.

      Following registration, the memory pointed to pDeviceElement can be freed.

   PARAMETERS:

      IN PCTXPNP_ELEMENT_HEADER         pDeviceElement           - Generic Citrix
                                                                   PnP Device element.
      IN PFN_CLPNP_DeviceChangeCallback pfn_DeviceChangeCallback - Device-change-
                                                                   notification callback.

   RETURN:

      WND_STATUS_SUCCESS - No error.
      Other values       - An error code, see ctxwnd.h for a list of all error
                           codes.
 ******************************************************************************/
VPSTATUS _VPAPI
Wnd_registerDeviceChangeCallback(PCTXPNP_ELEMENT_HEADER pDeviceElement,
                                 PFN_CLPNP_DEVICECHANGECALLBACK pfn_DeviceChangeCallback);


/*******************************************************************************
   Wnd_unregisterDeviceChangeCallback

      Unregisters a device-change-notification callback.

      The callback is unregistered for all device types for which it could have
      been previously registered by means of Wnd_registerDeviceChangeCallback.

   PARAMETERS:

      PFN_CLPNP_DeviceChangeCallback pfn_DeviceChangeCallback - Device-change-
                                                                   notification callback.
   RETURN:

      WND_STATUS_SUCCESS - No error.
      Other values       - An error code, see ctxwnd.h for a list of all error
                           codes.
 ******************************************************************************/
VPSTATUS _VPAPI
Wnd_unregisterDeviceChangeCallback(
#if defined(WIN32) 
        HANDLE,
#endif
        PFN_CLPNP_DEVICECHANGECALLBACK pfn_DeviceChangeCallback);

/* windows APIs defined in wndapi.c */

VPSTATUS _VPAPI Wnd_create(WND_TYPE     type,         /* the type of window to create */
           PVPRECT      pBounds,      /* the size and position of the new window */
           HND          hParent,      /* opt. parent window handle */
           PVOID        pSubscriber,  /* pointer to window event subscriber */
           PFNDELIVER   pfnDeliver, /* window event delivery function */
           HND          hThread,      /* thread to deliver events */
           PHND         phWindow );

VPSTATUS _VPAPI Wnd_setUserData(HND   hWindow,       /* handle of window */
                PVOID pUserData ) ;  /* pointer to user-defined structure */

VPSTATUS _VPAPI Wnd_getUserData(HND    hWindow,       /* handle of window */
                PPVOID ppUserData );  /* output var. for pointer to user-defined structure */
VPSTATUS _VPAPI Wnd_setParent(HND hWindow,   /* handle of window */
              HND hParent ); /* handle of new parent window */


VPSTATUS _VPAPI Wnd_setVisible(IN HND  hWindow, BOOL fVisible ) ;
VPSTATUS _VPAPI Wnd_setSize(HND hWindow, PVPSIZE pSize);
VPSTATUS _VPAPI Wnd_getSize(HND hWindow, PVPSIZE pSize);

VPSTATUS _VPAPI
Wnd_setPos(IN HND      hWindow,          /* handle of window */
           IN PVPPOINT pPos ) ;          /* new window position (x and y) */

VPSTATUS _VPAPI
Wnd_repaintRect(IN HND     hWindow,   /* handle of window */
                IN PVPRECT pRegion);   /* new window bounds */

VPSTATUS _VPAPI
Wnd_getRepaintRects( IN  HND      hWindow,    /* handle of window */
                     OUT PVPRECT  pRects,     /* a buffer for storing rects */
                     IN  INT16    cRects,     /* max number of rects to store in buffer */
                     OUT PINT16   pcRects ) ; /* rectangles stored */
                
VPSTATUS _VPAPI
Wnd_getObscuredRects( IN  HND     hWindow,    /* handle of window */
                      IN  PVPRECT pBounds,    /* optional bounding rect */
                      OUT PVPRECT pRects,     /* a buffer for storing rects */
                      IN  INT16   cRects,     /* max number of rects to store in buffer */
                      OUT PINT16  pcRects ) ; /* rectangles stored */

VPSTATUS _VPAPI
Wnd_setInputPreferences (IN PWND_INPUTPREFERENCES pPreferences); /* pointer to preferences */

VPSTATUS _VPAPI
Wnd_getKeyboardInfo(IN  UINT32 uiKeyboard, /* Kbd type code*/
                    IN  UINT32 uiLayout, /* Keyboard layout*/
                    IN  CHAR*  szKeyboardLayout, /* Layout */
                    OUT PWND_KEYBOARDINFO pKeybordInfo);/* Pointer to structure with kbd info*/

VPSTATUS _VPAPI
Wnd_getKeyboardState( PUINT32 );

VPSTATUS _VPAPI
Wnd_setKeyboardState( UINT32 );

VPSTATUS _VPAPI
Wnd_setCursor( HND hWindow, HGCURSOR hCursor ) ;

VPSTATUS _VPAPI
Wnd_setMousePos( INT32 iX, INT32 iY, HND hWindow );

VPSTATUS _VPAPI
Wnd_EjectPNP( CTXPNP_HND32 ContextHandle );

VPSTATUS _VPAPI
Wnd_requestFocus( HND hWindow );

VPSTATUS _VPAPI
Wnd_destroy(PHND phWindow ) ;    /* pointer to window handle */

#endif /* USE_WND_CALL_TABLE */

#ifdef __cplusplus
}
#endif


#endif /* __CTXWND_H_ */


