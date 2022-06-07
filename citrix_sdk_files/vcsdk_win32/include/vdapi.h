/***************************************************************************
*
*  VDAPI.H
*
*  This module contains external virtual driver API defines and structures
*
*  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/vdapi.h#1 $
*
*  Copyright 1998-2015 Citrix Systems, Inc.
*
****************************************************************************/

#ifndef __VDAPI_H__
#define __VDAPI_H__

#pragma once

#include <icaconst.h>
#include <dllapi.h>
#include <ica.h>

/*
*  Index into PD procedure array
*/
#define VDxQUERYINFORMATION      6
#define VDxSETINFORMATION        7
#define VDxCOUNT                 8   /* number of external procedures */

//=============================================================================
//   HPC Versions
//=============================================================================

// HPC_VD_API_VERSION_*
//
// These are the version identifiers for the VcSdk HPC engine API.  A VD will specify
// the API version it will use.  The engine will guarantee that VD's using older
// API versions will continue to operate properly.  The version number is ever-increasing.

typedef enum _HPC_VD_API_VERSION
{
	HPC_VD_API_VERSION_LEGACY =				0,				// legacy VDs
	HPC_VD_API_VERSION_V1 =					1,				// VcSdk API Version 1
	HPC_VD_API_VERSION_MAX =				2				// one higher than the largest version supported
} HPC_VD_API_VERSION;

//=============================================================================
//   Enums: For HPC API 1
//=============================================================================

// HPC_VD_OPTIONS
//
// Bit flags for options that a VD can set/request as part of the WDAPI_HPC_PROPERTIES
// WdSetInformation or WdQueryInformation calls. These options will apply to *all*
// VCs opened by a single VD.
//
// NOTE: Define these options so that the default value, corresponding to legacy operation
// of VCs, will be ZERO; i.e., set flag bits must be for new HPC operational features.

typedef enum _HPC_VD_OPTIONS
{
	HPC_VD_OPTIONS_NO_POLLING =		0x0001,		// Flag indicating that channels on this VD do not require send data polling
	HPC_VD_OPTIONS_NO_COMPRESSION =	0x0002		// Flag indicating that channels on this VD send data that does not need reducer compression
} HPC_VD_OPTIONS;

// SENDDATA:  This defines the flags used in Hpc_Comm_ApiSendData, field ulFlags.
// Each of the flags is ORed together in the field.

typedef enum _SENDDATA
{
	SENDDATA_NOTIFY =			0x0001	// If this flag is set, the VD will be notified when it can retry 
										// sending following a CLIENT_ERROR_NO_OUTBUFS error.  The notification occurs via the
										// DriverPoll method.
} SENDDATA;

/*
*  VdOpen structure
*/
typedef struct _VDOPEN
{
	LPVOID pIniSection;
	PDLLLINK pWdLink;
	/* This field can be either a bit mask of supported channels (b0=0) OR 
	* it can be the actual number of a supported channel. 
	* The correct interpretation will be determined by the WD and hence 
	* this field should not be used to set/get any information about the supported channels. */
	ULONG ChannelMask;
	PLIBPROCEDURE pfnWFEngPoll;
	PLIBPROCEDURE pfnStatusMsgProc;
	HND hICAEng;
} VDOPEN, FAR * PVDOPEN;


/*
*  VdInformationClass enum
*   additional information classes can be defined for a given
*   VD, as long as the number used is greater than that generated
*   by this enum list
*/

typedef enum _VDINFOCLASS
{
#ifndef unix
	VdLastError,
#endif /* unix */
	VdKillFocus,
	VdSetFocus,
#ifndef unix
	VdMousePosition,
#endif /* unix */
	VdThinWireCache,
	VdWinCEClipboardCheck,		//  Used by WinCE to check for clipboard changes
	VdDisableModule,
	VdFlush,
	VdInitWindow,
	VdDestroyWindow,
#ifndef unix
	VdPaint,
#endif /* unix */
	VdThinwireStack,
	VdRealizePaletteFG,			// inform client to realize it's foreground palette
	VdRealizePaletteBG,			// inform client to realize it's background palette
	VdInactivate,				// client is about to lose input focus
#ifndef unix
	VdGetSecurityAccess,		// cdm security info
	VdSetSecurityAccess,		// cdm security info
	VdGetAudioSecurityAccess,	// cdm audio security info
	VdSetAudioSecurityAccess,	// cdm audio security info
#endif /* unix */
	VdGetPDASecurityAccess,		// cdm PDA security info
	VdSetPDASecurityAccess,		// cdm PDA security info
#ifndef unix
	VdGetTWNSecurityAccess,		// cdm TWN security info
	VdSetTWNSecurityAccess,		// cdm TWN security info
#endif /* unix */
	VdSendLogoff,
	VdCCShutdown,
	VdSeamlessHostCommand,		// Seamless command call
	VdSeamlessQueryInformation,	// Seamless query call
	VdWindowGotFocus,
	VdSetCursor,				// Set: New cursor from TW. Data - PHGCURSOR
	VdSetCursorPos,				// Set: New cursor position. Data - VPPOINT
	VdEnableState,				// Set/Get driver state (enabled/disabled)
	VdIcaControlCommand,
#ifndef unix
	VdVirtualChannel,			// Set/Get virtual channel data
	VdWorkArea,					// Set the work area of the application
#endif /* unix */
	VdSupportHighThroughput,
#ifndef unix
	VdRenderingMode,			// Set/Get the rendering mode (headless client)
#endif /* unix */
	VdPauseResume,				// Pause/Resume commands
#ifdef BLT_IS_EXPENSIVE
	VdRedrawNotify,				// Overdrawing suppression.
#endif
	VdDisplayCaps,				// Get/Set display capabilities and/or preferred mode
	VdICOSeamlessFunctions,		// Get seamless functions for ICO
	VdPnP,						// Set: Send CLPNP_COMMAND commands inbetween the control VC and the implementation VC (such as VIRTUAL_CCM)
	/* Support for EUEM (Darwin Release For Ohio). This code has been added or
	* changed to support End User Experience Monitoring functionality. Please do
	* not change the code in this section without consulting the EUEM team
	*  Email (at the time of change) "#ENG - Darwin Dev" 
	*/
	VdEuemStartupTimes,			// Set: EUEM: pass the startup times of shared sessions to the EUEM VD
	VdEuemTwCallback,			// Set: register the EUEM ICA roundtrip callback
								// function from the thinwire VC to the EUEM VC
								// End of EUEM support section
	VdResizeHotBitmapCache,		// Set: Tell thinwire driver to resize the hot bitmap cache
	VdSetMonitorLayout,			// Set: Tell thinwire driver to update monitor layout info
	VdGUSBGainFocus,			// Set: Tell Generic USB driver that engine has gained keyboard focus
	VdGUSBLoseFocus,			// Set: Tell Generic USB driver that engine has lost keyboard focus
	VdCreateChannelComms,		// Query: Provide the driver with a pipe to communicate directly with
	VdGetPNPSecurityAccess,		// usb PNP security info
	VdSetPNPSecurityAccess,		// usb PNP security info
	VdReverseSeamless,			// For use with RS specific calls
	VdInfoRequest,				// Used to request information from a VD
    VdReverseSeamlessPartial,   // partial RS packet data used to form a complete RS VC packet
	VdEuemNotifyReconnect,		// Notify EUEM about a reconnect
    VdCHAEnabled,                // Notify Drivers about Enabling/Disabling CHA based on CST recommendation
	VdMTCommand,
    VdSendMouseData,            // Mouse data packets to be sent to host when using VC for mouse data
    VdSendKeyboardData,          // Keyboard type and codes to be sent to host when using VC for keyboard data
	VdSendTouchData,
	VdGUSBSecondAppStarts,
	VdCTXIMEHotKeySetIMEModeInApp,
	VdCTXIMEHotKeySetIMEModeInCDSBar,
  	VdCTXIMEQueryInformation,
  	VdCTXIMESeamlessQueryInformation,
  	VdCTXIMESetDispWMInfo,
  	VdCTXIMESetSeamlessWMInfo,
	VdSeamlessResumeLaterCapEnabled
} VDINFOCLASS;

/*
*  VdQueryInformation structure
*/
typedef struct _VDQUERYINFORMATION
{
	VDINFOCLASS VdInformationClass;
	LPVOID pVdInformation;
	int VdInformationLength;
	int VdReturnLength;
} VDQUERYINFORMATION, * PVDQUERYINFORMATION;

/*
*  VdSetInformation structure
*/
typedef struct _VDSETINFORMATION
{
	VDINFOCLASS VdInformationClass;
	LPVOID pVdInformation;
	int VdInformationLength;
} VDSETINFORMATION, * PVDSETINFORMATION;

/*
*  VdLastError
*/
typedef struct _VDLASTERROR
{
	int Error;
	char Message[ MAX_ERRORMESSAGE ];
} VDLASTERROR, * PVDLASTERROR;

/*
* VD Flush
*/
typedef struct _VDFLUSH
{
	UCHAR Channel;
	UCHAR Mask;
} VDFLUSH, * PVDFLUSH;

#define  VDFLUSH_MASK_PURGEINPUT    0x01
#define  VDFLUSH_MASK_PURGEOUTPUT   0x02

#endif /* __VDAPI_H__ */
