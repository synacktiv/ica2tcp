/*************************************************************************
 *
 *  platfix.h
 *
 *  Fixups that allow the Unix client to build with new common include
 *  files that are used.
 *
 *  Version $Id$
 *
 * Copyright 1999-2017 Citrix Systems, Inc.  All Rights Reserved.
 *
 ************************************************************************/
#ifndef _UNIX_PLATFIX_H_  
#define _UNIX_PLATFIX_H_

/*
 *-----------------------------------------------------------------------------
 *  UNIFICATION : Defines needed to get Unix client working with common headers                    
 *-----------------------------------------------------------------------------
 */

// This is also defined in CL Mgr so it is a re-define when including CL Mgr headers
#ifndef _stricmp
#define _stricmp        stricmp
#endif

/*=============================================================================
 ==  AppServer INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

/* Event loop/video/user input plugin file. */

#define INI_KVME_PLUGIN                 "KVMEPlugin"
#define DEF_KVME_PLUGIN                 "KVMEPlugin.so"

/* Can this X display work with the X11 shared memory extension? */

#define INI_LOCAL_DISPLAY_NAMES         "LocalDisplayNames"
#define DEF_LOCAL_DISPLAY_NAMES         ":unix"

/* Related to CPR 890 fix shadowing when session size is same as screen size */

#define INI_NO_WINDOW_MANAGER		"NoWindowManager"
#define DEF_NO_WINDOW_MANAGER		FALSE
#define INI_WM_WIDTH_ALLOWANCE		"WindowManagerWidthAllowance"
#define DEF_WM_WIDTH_ALLOWANCE		20
#define INI_WM_HEIGHT_ALLOWANCE		"WindowManagerHeightAllowance"
#define DEF_WM_HEIGHT_ALLOWANCE		60

/* Do we use window or pixmap icons for our windows? */

#define INI_USEICONWINDOW               "UseIconWindow"
#define DEF_USEICONWINDOW               FALSE

#define INI_CDM_IGNORE_FILECHANGESIZE	"IgnoreFileChangeSize"	/* Ignore FileChangeSize requests */


#define INI_IGNOREERRORS "IgnoreErrors"
#define DEF_IGNOREERRORS ""

#define INI_IGNOREXERRORS "IgnoreXErrors"
#define DEF_IGNOREXERRORS ""

#define INI_STOPONUNMAP   "StopOnUnmap" /* Controls ICA PACKET_STOP_REQUEST. */
#define DEF_STOPONUNMAP   True

/* Unix Inter-Client-Communication (ICC) */
#define INI_ENABLE_ICC  "EnableICC"
#define DEF_ENABLE_ICC  TRUE
#define INI_ATTEMPT_CROSSPLATFORM_SESSION_REUSE  "AttemptCrossPlatformSessionReuse"
#define DEF_ATTEMPT_CROSSPLATFORM_SESSION_REUSE  FALSE

/* Unix Seamless */
#define INI_BYPASS_WM "BypassWindowManager"
#define DEF_BYPASS_WM FALSE
#define INI_LOCAL_DECOR "LocalWMDecorations"
#define DEF_LOCAL_DECOR FALSE
#define INI_INSTALL_COLORMAP "InstallColormap"
#define DEF_INSTALL_COLORMAP TRUE
#define INI_SCN_TIMEOUT "SCNTimeout"
#define DEF_SCN_TIMEOUT 500
#define INI_SCN_DELAY "SCNDelay"
#define DEF_SCN_DELAY 500
#define INI_SCN_EXPECT_CHANGEW_TIMEOUT "ExpectChangeWTimeout"
#define DEF_SCN_EXPECT_CHANGEW_TIMEOUT 0
#define INI_EXPECTED_CWA_TIMEOUT "ExpectedCWATimeout"
#define DEF_EXPECTED_CWA_TIMEOUT 5000
#define INI_WINDOW_MANAGER_MOVE_TIMEOUT "WindowManagerMoveTimeout"
#define DEF_WINDOW_MANAGER_MOVE_TIMEOUT 500
#define INI_WINDOW_MANAGER_MOVE_IGNORED "WindowManagerMoveIgnored"
#define DEF_WINDOW_MANAGER_MOVE_IGNORED FALSE
#define INI_SET_TWI_FOCUS		"SetTWIFocus"
#define DEF_SET_TWI_FOCUS		FALSE

/* These control how we move/resize Seamless windows. */

#define INI_TWI_MOVE_RESIZE_TYPE        "TWIMoveResizeType"
#define USE_XDG_SEAMLESS_MOVE           1
#define USE_XDG_SEAMLESS_RESIZE         2
#define DEF_TWI_MOVE_RESIZE_TYPE        USE_XDG_SEAMLESS_MOVE

/* 
 * This controls how we hide the window on server when we move/resize client-side 
 * seamless window controlled by the window manager(TWIMoveResizeType=1,2,3).
 */

#define TWI_HIDE_AS_ICON		1
#define TWI_HIDE_IN_CORNER		2
#define TWI_DO_NOT_HIDE         3

#define INI_TWI_MOVERESIZE_HIDE_WINDOW_TYPE	"TWIMoveResizeHideWindowType"
#define DEF_TWI_MOVERESIZE_HIDE_WINDOW_TYPE	TWI_HIDE_AS_ICON

/* This controls how we hide windows on server when we switch between workspaces. */

#define INI_TWI_WS_HIDE_WINDOW_TYPE	"TWIWSHideWindowType"
#define DEF_TWI_WS_HIDE_WINDOW_TYPE	TWI_HIDE_AS_ICON

/* How to implement Seamless "flash icon" message.
 * See twi_init.c for what the values mean.
 */

#define INI_TWI_FLASH_METHOD            "TWIFlashMethod"
#define DEF_TWI_FLASH_METHOD            3


#define INI_ICC_TIMEOUT                 "ICCTimeout"
#define DEF_ICC_TIMEOUT                 13
#define INI_XFREE_SHAPE_FIX_LEVEL       "XFree86ShapeFixLevel"
#define DEF_XFREE_SHAPE_FIX_LEVEL       40300001
#define INI_DESKTOP_LOCAL_ICONS         "PublishedDesktopLocalIcons"
#define DEF_DESKTOP_LOCAL_ICONS         TRUE


#define INI_USE_NETSCAPE_PROXY_SETTINGS "UseNetscapeProxySettings"
#define DEF_USE_NETSCAPE_PROXY_SETTINGS TRUE


/*
 *  Client Audio Mapping
 */
 
#define INI_VDCAM30			"ClientAudio"

#define INI_AUDIODEVICE			"AudioDevice"
#if defined(Solaris) || defined(SolarisX86) || defined(netbsd)
#define DEF_AUDIODEVICE			"/dev/audio"
#else
#define DEF_AUDIODEVICE			"<none>"
#endif /* Machine specific audio device */

#define INI_AUDIO_BUFFERSIZEMILLISECONDS "AudioBufferSizeMilliSeconds"
#define DEF_AUDIO_BUFFERSIZEMILLISECONDS 200

#define INI_AUDIO_LATENCY_CONTROL_ENABLED "AudioLatencyControlEnabled"
#define DEF_AUDIO_LATENCY_CONTROL_ENABLED TRUE

#define INI_AUDIO_MAX_LATENCY "AudioMaxLatency"
#define DEF_AUDIO_MAX_LATENCY 300

#define INI_AUDIO_LATENCY_CORRECTION_INTERVAL "AudioLatencyCorrectionInterval"
#define DEF_AUDIO_LATENCY_CORRECTION_INTERVAL 300

#define INI_AUDIO_TEMP_LATENCY_BOOST "AudioTempLatencyBoost"
#define DEF_AUDIO_TEMP_LATENCY_BOOST 300

#define INI_CAM_AUDIOSINK_NAME "GSTAudioSinkName"
#define DEF_CAM_AUDIOSINK_NAME "autoaudiosink"

#define INI_CAM_AUDIOSRC_NAME "GSTAudioSrcName"
#define DEF_CAM_AUDIOSRC_NAME "autoaudiosrc"

#define INI_GST_SPEEX_BUFFERING_LATENCY "GSTSpeexBufferingLatency"
#define DEF_GST_SPEEX_BUFFERING_LATENCY 50

#define INI_GST_VORBIS_BUFFERING_LATENCY "GSTVorbisBufferingLatency"
#define DEF_GST_VORBIS_BUFFERING_LATENCY 150

#define INI_GST_AUDIO_NO_CLOCK "GSTUseNoClock"
#define DEF_GST_AUDIO_NO_CLOCK FALSE

#define INI_ALLOWAUDIOINPUT	"AllowAudioInput"
#define DEF_ALLOWAUDIOINPUT	FALSE

#define INI_AUDIOLEGACYLIBRARY "AudioLegacyLibrary"
#define DEF_AUDIOLEGACYLIBRARY "AUDALSA.DLL"

#define INI_AUDIOPULSELIBRARY "AudioPulseLibrary"
#define DEF_AUDIOPULSELIBRARY "AUDPULSE.DLL"

#define INI_PULSEAUDIO_RECORD_MONITOR_ENABLE "PulseAudioRecordMonitorEnable"
#define DEF_PULSEAUDIO_RECORD_MONITOR_ENABLE FALSE

/* Unix Smart Card VD entries */
#define INI_PCSC_LIBRARY_NAME		"PCSCLibraryName"
#define DEF_PCSC_LIBRARY_NAME		"libpcsclite.so"
#define INI_PCSC_CODEPAGE               "PCSCCodePage"
#define DEF_PCSC_CODEPAGE               0
#define INI_UCSCWA                      "UCSCWAMap"
#define DEF_UCSCWA                      0

/*=============================================================================
 ==   Transport Driver INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

/* Digital UNIX & HP use the address of a running network controller as the 
 * hostid for licensing. If INI_IDENTIFIER has been set it uses that controller 
 * provided it is up. If INI_IDENTIFIER is not set it uses the first it finds. 
 * If none is found an error is reported.
 */
#define INI_IDENTIFIER		"IdentificationController" /* "ln0", "tra0" etc */

#ifdef Linux
#define DEF_IDENTIFIER		"eth0"			/* Prefer eth0 */
#else
#define DEF_IDENTIFIER		""			/* Use first controller */
#endif

#define IDENTIFIER_LENGTH	10			/* longest so far is "xna0" */

/*=============================================================================
 ==  Configuration INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

#define INI_NAMERESOLVER     "NameResolverTCP"
#define INI_DRIVERNAME	     "DriverName"
#define INI_NAMERESOLVERW    "NameResolverHTTP"
#define NR_EMBEDDED

/*=============================================================================
 ==  Client Name INI key (INI_) and defaults (DEF_)
 ==     The ClientName is in <boot-drive>:\WFCNAME.INI for DOS and WIN16
 ==                and is in <system-drive>:\WFCNAME.INI for WIN32
 ============================================================================*/

#define INI_COMPORTCOUNT	"LastComPortNum"			/* Used by CSM */
#define DEF_COMPORTCOUNT      	0						/* Used by CSM */

#define INI_COMPORTDEVICE	"ComPort"					/* Used by CSM */
#define DEF_COMPORTDEVICE      	""							/* Used by CSM */

#define INI_CPMUNICODEENABLED	"UnicodeEnabled"
#define DEF_CPMUNICODEENABLED   TRUE

/*
 * Unix Printer entries
 */
/* Colon separated list of printers to avoid running a command to look for them */
#define INI_CPM_PRINTER_LIST	"ClientPrinterList"
#define DEF_CPM_PRINTER_LIST	""

/* Command lines to print a file, %s will be replaced by the printer name */

#define INI_CPM_UNIX_PRINT_CMD	"UnixPrintCommand"
#define INI_CPM_MF_PRINT_CMD	"MFPrintCommand"
#ifdef UNIX_BSD_PRINTING
#define DEF_CPM_UNIX_PRINT_CMD	"lpr -l -P\"%s\""
#define DEF_CPM_MF_PRINT_CMD	"lpr -P\"%s\""
#else
#define DEF_CPM_UNIX_PRINT_CMD	"lp -d\"%s\""
#define DEF_CPM_MF_PRINT_CMD	DEF_CPM_UNIX_PRINT_CMD
#endif

/* Incident 30081638 (AT&T). Allow default printer to be specified or determined */
#define INI_DEFAULT_PRINTER		"DefaultPrinter"
#define DEF_DEFAULT_PRINTER		""
#define INI_DEFAULT_PRINTER_DRIVER	"DefaultPrinterDriver"
#define DEF_DEFAULT_PRINTER_DRIVER	""
#define INI_PRINTER_QUERY_REFRESH_TIME	"PrinterQueryRefreshTime"
#define DEF_PRINTER_QUERY_REFRESH_TIME	60

/* Determine whether /etc/printcap should be parsed for list of printers. */
#define INI_CPM_USE_PRINTCAP    "UsePrintcap"
#define DEF_CPM_USE_PRINTCAP    FALSE


/*=============================================================================
 ==   Transport Driver INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

#define INI_TCPGROUPNAME "TcpGroupName"	/* Used by CSM */
#define DEF_TCPGROUPNAME ""				/* Used by CSM */

/* DPP 9/5/96 - support for turning on TcpNodelay */
#define INI_TCPNODELAY  "TcpNoDelay"
#define DEF_TCPNODELAY  FALSE

/* Citrix-implemented timeout for gethostbyname */
#define INI_HOSTLOOKUPTIMEOUT "HostLookupTimeout"
#define DEF_HOSTLOOKUPTIMEOUT 5 /* seconds */

/*=============================================================================
 ==   Keyboard-related INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

#define INI_BALANCE_SHIFTS      "BalanceShiftKeys"
#define DEF_BALANCE_SHIFTS      TRUE

#define INI_BALANCED_SHIFTS     "BalancedShiftMask"     /* LED_xxx macros. */

#define INI_ECHO_SHIFTS         "EchoShiftKeys"
#define DEF_ECHO_SHIFTS         FALSE

#define INI_KEYMAPFILE	     	"KeyboardMappingFile"
#define DEF_KEYMAPFILE       	""

#define INI_OVERLINE2TILDE        "ReplaceOverlineWithTilde"
#define DEF_OVERLINE2TILDE        FALSE

#define INI_USELOCALIM            "UseLocalIM"
#define DEF_USELOCALIM            FALSE

/* Two keyboard mapping features that are mainly for Solaris. */

#define INI_LOCALE_KEY_MAPPING	  "LocaleKeyMapping"
#define DEF_LOCALE_KEY_MAPPING	  FALSE

#define INI_SUPERMETA_TO_WIN_KEYS "SuperMetaToWinKeys"
#define DEF_SUPERMETA_TO_WIN_KEYS FALSE
#define INI_RIGHTSUPERMETA_TO_MENU_KEY "RightSuperMetaToMenuKey"
#define DEF_RIGHTSUPERMETA_TO_MENU_KEY FALSE

#define INI_USE_SUN_EDIT_KEYS     "UseSunEditKeys"
#define DEF_USE_SUN_EDIT_KEYS     TRUE

#define INI_USE_CTRLALTEND        "UseCtrlAltEnd"
#define DEF_USE_CTRLALTEND        FALSE

/*=============================================================================
 ==   Mouse-related INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

#define INI_MAPMOUSEBUTTON2	"MapMouseButton2"
#define DEF_MAPMOUSEBUTTON2	FALSE

#define INI_MOUSESENDSCONTROLV  "MouseSendsControlV"
#define DEF_MOUSESENDSCONTROLV  FALSE

/*=============================================================================
 ==  AppServer INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

#define INI_MINIMIZE	    "MinimizeOnUse"	/* Used by CSM */
#define DEF_MINIMIZE	    FALSE			/* Used by CSM */
#define INI_POPUP	    "PopupOnExit"	/* Used by CSM */
#define DEF_POPUP	    FALSE			/* Used by CSM */

#define INI_APPROXIMATE_COLORS "ApproximateColors"	/* Used by CSM */
#define DEF_APPROXIMATE_COLORS FALSE				/* Used by CSM */

#define INI_REQUIRE_BLACK0_WHITE255 "RequireBlack0White255"
#define DEF_REQUIRE_BLACK0_WHITE255 TRUE

#define INI_APPROX_COLORS_USE_DEFAULT  "UseDefaultSettingForColormap"	/* Used by CSM */
#define DEF_APPROX_COLORS_USE_DEFAULT  TRUE								/* Used by CSM */

#define INI_FORCE_EMBEDDED_COLORMAP_SWITCH	"ForceEmbeddedColormapSwitch"
#define DEF_FORCE_EMBEDDED_COLORMAP_SWITCH	FALSE

/*
 *  vdcm.dll
 */

/*  Part of WFCLIENT section */
#define INI_CM_UPDATESALLOWED    "UpdatesAllowed"
#define DEF_CM_UPDATESALLOWED    TRUE

#define INI_CM_FORCEUPDATE		"ForceUpdate"
#define DEF_CM_FORCEUPDATE		TRUE

/* Part of ClientManagement section*/
#define INI_CM_VDSECTION        "ClientManagement"
#define DEF_CM_VDSECTION        TRUE


/*
 *  vdtw30.dll
 */

#define INI_USERVISUALCLASS         "UserVisualClass"
#define DEF_USERVISUALCLASS         ""
#define INI_USERVISUALID            "UserVisualID"
#define DEF_USERVISUALID            "-1"
#define INI_LARGECACHESIZE_IN_K "LargeCacheSizeInK"
#define DEF_LARGECACHESIZE_IN_K 2048
#define INI_ALLOW_SHARED_16COLORS "AllowShared16Colors"
#define DEF_ALLOW_SHARED_16COLORS TRUE

#define INI_REDRAWTIMER    "RedrawTimer"
#define DEF_REDRAWTIMER    1000

#define INI_USEFULLSCREEN  "UseFullScreen"
#define DEF_USEFULLSCREEN  FALSE

#define INI_USE_XDG_FS     "UseXDGFullScreen"
#define DEF_USE_XDG_FS     True

#define INI_DISABLE_DBE    "DisableDbe"
#define DEF_DISABLE_DBE    FALSE

#define INI_DBE_MODE       "DbeMode"
#define DEF_DBE_MODE	   "XdbeCopied"


#define INI_TRUE           "True"
#define INI_FALSE          "False"

/* Monochrome cursor conversion. */

#define INI_CURSOR_STIPPLE "CursorStipple"
#define DEF_CURSOR_STIPPLE "aaaa,5555"

/* Multimod determination by X extension. */

#define INI_MM_EXTENSION "MMExtension%d"
#define DEF_MM_EXTENSION ""
#define INI_MM_EXTENSION_VAL "MMExtensionVal%d"

#define INI_DISABLE_XRENDER             "DisableXRender"
#define DEF_DISABLE_XRENDER             FALSE

/*
 * Drive Mapping keys - each should have the drive letter appended, eg DriveEnabledA,
 * DriveReadAccessB
 */
#define INI_DRIVE_PATH			"DrivePath"
#define INI_DRIVE_ENABLED		"DriveEnabled"
#define INI_DRIVE_READ_ACCESS	"DriveReadAccess"
#define INI_DRIVE_WRITE_ACCESS	"DriveWriteAccess"

#define INI_DRIVE_RW_YES		0		/* Can read/write */
#define INI_DRIVE_RW_NO			1		/* Can't read/write */
#define INI_DRIVE_RW_QUERY		2		/* Ask user */

#define INI_CDM_MAX_DRIVES		26	/* Max number of drives 'A' - 'Z' */
#define INI_CDM_START_DRIVE_LETTER	'A'	/* Drive letter to start from */
#define INI_CDM_FLOPPY_DRIVE		0	/* FLOPPY drive - just here for completeness really */
#define INI_CDM_CDROM_DRIVE		1	/* CDROM drive - special case for denying write access */
#define INI_CDM_BOOT_DRIVE		2	/* Boot volume - always mapped to C: */

#ifdef macintosh
#define INI_CDM_FIRST_PATH_DRIVE	3	/* Drive letter of first 'path' drive
						(vs floppy/cdrom/boot volume) */
#endif


/* Version numbers stored in the WFClient Section of the ini files. */
#define INI_ISVERSION        "ISVersion"
#define DEF_ISVERSION        0
#define ISVERSION_1	     DEF_ISVERSION	/* Not present */
#define ISVERSION_2	     200		/* 2.0.0 */
#define ISVERSION_3	     300		/* 3.0.0 */
#define ISVERSION_4	     400		/* 4.0.0 */

#define CURRENT_ISVERSION    ISVERSION_3

/*
 *  vdcdm30.dll
 */

#define INI_VDCDM30		"ClientDrive"

#define INI_VIRTUAL_DRIVER   "VirtualDriver"

/*
 * vdcom30.dll
 */
#define INI_VDCCM30     "ClientComm"

/*
 *  vdcpm30.dll
 */

/* Which type of printer is in use */
#define INI_PRINTER_TYPE		"PrinterType"
#define DEF_PRINTER_TYPE		PRINTER_TYPE_NONE

/* For debugging - keep the spool file for examining - not normally set! */
#define INI_PRINT_KEEP_FILE		"PrinterKeepFile"
#define DEF_PRINT_KEEP_FILE		FALSE

/* Printer types supported */
#define PRINTER_TYPE_NONE		0
#define PRINTER_TYPE_POSTSCRIPT	1
#define PRINTER_TYPE_DIRECT		2

#define INI_VDCPM30		"ClientPrinter"

#define INI_CPMUNICODEENABLED	"UnicodeEnabled"
#define DEF_CPMUNICODEENABLED   TRUE

/*
 * Printer entries
 */

/* PostScript printer keys */
#define INI_PS_DRIVER			"PrinterPSDriver"
#define DEF_PS_DRIVER			"Generic PostScript Printer"

/* What do we map a generic PS printer to? */
#define INI_PS_GENERIC			"GenericPSMapping"
#define DEF_PS_GENERIC			"Apple LaserWriter v23.0"

/* Which type of printer is in use */
#define INI_PRINTER_TYPE		"PrinterType"
#define DEF_PRINTER_TYPE		PRINTER_TYPE_NONE


/* for hp800 audio */
#define INI_AUDIO_TIMEOUT		"AudioTimeout"
#define DEF_AUDIO_TIMEOUT		5000

/*=============================================================================
 ==   String typedefs (string length + 1 for NULL terminator)
 ============================================================================*/

#ifdef DOS
#define WFENG_NUM_HOTKEYS                           2
#elif defined(MacOrUnix)
#define WFENG_NUM_HOTKEYS                           15
#else
#define WFENG_NUM_HOTKEYS                           8
#endif

#define HOTKEYNAME_LENGTH       20


#define KEYBOARDMAPPINGFILE_LENGTH	80
#define KEYBOARDDESCRIPTION_LENGTH 	80

typedef CHAR KEYBOARDMAPPINGFILE[ KEYBOARDMAPPINGFILE_LENGTH + 1 ];
typedef CHAR KEYBOARDDESCRIPTION[ KEYBOARDDESCRIPTION_LENGTH + 1 ];

/*============================================================================
 ===  Used for the com port device specification feature on prefs form
 ===========================================================================*/                              
#define MAX_COM_PORTS		32
#define CCM_MAXFILENAMESIZE 	256
#define CCM_MAXPORTNAMESIZE	20

typedef CHAR CCMFILENAME[CCM_MAXFILENAMESIZE + 1];
typedef CHAR CCMPORTNAME[CCM_MAXPORTNAMESIZE + 1];

/*============================================================================
 ===  Used for the business recovery feature added to server list 
 ===========================================================================*/                              
#define MAX_BROWSERADDRESSLIST		15	/* max number of servers and seperators in the server address list */
#define SERVERS_IN_GROUP		5
#define MAX_BROWSER_GROUPS		(MAX_BROWSERADDRESSLIST/SERVERS_IN_GROUP)	/* number of browser group names */

/*
 * Drive Mapping keys - each should have the drive letter appended, eg DriveEnabledA,
 * DriveReadAccessB
 */
#define INI_DRIVE_RW_YES		0		/* Can read/write */
#define INI_DRIVE_RW_NO			1		/* Can't read/write */


/* 
 * Com Port Mapping User Definable Settings
 * 
 */

#define INI_COMPORTCOUNT	"LastComPortNum"
#define INI_COMPORTDEVICE	"ComPort"

#define INI_CCM_COMPORT_STEM INI_COMPORTDEVICE 
#define NULL_CCM_COMPORT      ""
#define INI_CCM_LAST_PORTNO INI_COMPORTCOUNT 
#define DEF_CCM_LAST_PORTNO 0	

#if defined(LINUX_OS)
#define INI_CCM_HOLD_PORTS_OPEN	"HoldComPortsOpen"
#define DEF_CCM_HOLD_PORTS_OPEN	FALSE
#else
#define INI_CCM_HOLD_PORTS_OPEN	"HoldComPortsOpen"
#define DEF_CCM_HOLD_PORTS_OPEN	TRUE
#endif

/* for testing with pseudo-terminals */
#define INI_CCM_IGNORE_PORT_OPEN_ERROR	"IgnoreComPortOpenError"
#define DEF_CCM_IGNORE_PORT_OPEN_ERROR	FALSE

/*=============================================================================
 ==   Protocol Driver INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

#define INI_PROTOCOL_SUPPORT  "ProtocolSupport"

/* INI file names for built-in PDs */

#define INI_PDFRAME	"Framing"
#define INI_PDCRYPT	"Encrypt"
#define INI_PDRELI	"Reliable"
#define INI_PDRFRAME	"RFrame"
#define INI_PDMODEM	"Modem"
#define INI_PDCOMPR	"Compress"
#define INI_PDOEMFLTR	"OemFilter"
#define INI_PDOEMCONN	"OemConnect"

/* Should be cdmapi.h */

/*
	CDM Find data
*/

#define MAX_WIN_LONGNAME_PATH	256
#define MAX_WIN_ALTERNATE_PATH	14
#define LENGTH_8_DOT_3			12
/*=============================================================================
 ==  Configuration INI keys (INI_) and defaults (DEF_)
 ============================================================================*/

#define INI_WDTYPE            "WinStationDriver"
#define DEF_WDTYPE            INI_ICA30

#define MAXFILEPATH 		255		/* Used by CSM */

#define INI_PCKEYBOARDTYPE  "KeyboardType"
#define DEF_PCKEYBOARDTYPE  "(Default)"

#define MAX_PRINTDRIVER_LIST 	50   /* Used by CSM */

#define INI_DIRECT_DRIVER_LIST  "DirectPrinterDrivers"    /* Used by CSM */
#define INI_PS_DRIVER_LIST      "PostScriptPrinterDrivers"   /* Used by CSM */

#define HOT_KEY1	0		/* Used by CSM */
#define HOT_KEY2	1		/* Used by CSM */
#define HOT_KEY3	2		/* Used by CSM */
#define HOT_KEY4	3		/* Used by CSM */
#define HOT_KEY5	4		/* Used by CSM */
#define HOT_KEY6	5		/* Used by CSM */
#define HOT_KEY7	6		/* Used by CSM */
#define HOT_KEY8	7		/* Used by CSM */
#define HOT_KEY9	8		/* Used by CSM */
#define HOT_KEY10	9		/* Used by CSM */
#define HOT_KEY11	10		/* Used by CSM */
#define HOT_KEY12	11		/* Used by CSM */
#define HOT_KEY13	12		/* Used by CSM */
#define HOT_KEY14	13		/* Used by CSM */
#define HOT_KEY15       14              /* Used by CSM */


/* Should be in vrtlclss.h */

/*
 *  Virtual i/o channel ids
 *  NOTE: don't change the order of this structure
 */
typedef enum _VIRTUALCLASSX {
	Virtual_License   = 18,				  /* License VD */
	Virtual_Cam       = 19,				  /* Client Audio Mapping */
} VIRTUALCLASSX;

/* Where should these be defined ? */
/*
 * The string that will be pre-pended to the X display's vendor string
 * when looking for an INI section specifying the server's multimod behaviour.
 */

#define INI_MMOD_SECT_PFIX  "Xdpy - "

/* This single key is looked up in that section to identify a
 * non-multimod server.
 */

#define INI_IS_MULTIMOD                "ServerDoesMultiMod"
#define DEF_IS_MULTIMOD                TRUE

/*
 * original Content Redirection (aks Server File Type Association) settings
 */
#define INI_CR_ENABLED			"CREnabled"
#define DEF_CR_ENABLED			TRUE
#define INI_CR_BROWSER_PATH		"CRBrowserPath"
#define DEF_CR_BROWSER_PATH		""
#define INI_CR_BROWSER_CMD		"CRBrowserCommand"
#define DEF_CR_BROWSER_CMD		"$ICAROOT/util/nslaunch %s || ${BROWSER:=netscape} %s || mozilla %s"
#define INI_CR_N_BROWSER_PERCENT_S	"CRBrowserPercentS"
#define DEF_CR_N_BROWSER_PERCENT_S	3
#define INI_CR_BROWSER_ACCEPT_URL	"CRBrowserAcceptURLtype"
#define DEF_CR_BROWSER_ACCEPT_URL	"http,https"
#define INI_CR_BROWSER_REJECT_URL	"CRBrowserRejectURLtype"
#define DEF_CR_BROWSER_REJECT_URL	""
#define INI_CR_PLAYER_PATH		"CRPlayerPath"
#define DEF_CR_PLAYER_PATH		""
#define INI_CR_PLAYER_CMD		"CRPlayerCommand"
#define DEF_CR_PLAYER_CMD		"realplay %s"
#define INI_CR_N_PLAYER_PERCENT_S	"CRPlayerPercentS"
#define DEF_CR_N_PLAYER_PERCENT_S	1
#define INI_CR_PLAYER_ACCEPT_URL	"CRPlayerAcceptURLtype"
#define DEF_CR_PLAYER_ACCEPT_URL	"rtsp,rtspu,pnm"
#define INI_CR_PLAYER_REJECT_URL	"CRPlayerRejectURLtype"
#define DEF_CR_PLAYER_REJECT_URL	"mms"

/*
 * Content Redirection settings for 'Flexible Server to Client Content Redirection). 
 * See CPR 57450
 */
#define INI_CR_SCHEME                 "ContentRedirectionScheme"
#define DEF_CR_SCHEME                 ""
#define INI_CR_PATH                   "Path"
#define DEF_CR_PATH                   ""
#define INI_CR_CMD                    "Command"
#define DEF_CR_CMD                    ""
#define INI_CR_ACCEPT_URL_TYPE        "AcceptURLType"
#define DEF_CR_ACCEPT_URL_TYPE        ""
#define INI_CR_REJECT_URL_TYPE        "RejectURLType"
#define DEF_CR_REJECT_URL_TYPE        ""


/*
 * Browser proxy parsing.
 */
#define INI_USEMRUBROWSERPREFS	"UseMRUBrowserPrefs"
#define DEF_USEMRUBROWSERPREFS	TRUE

/*
 * file associations
 */
#define INI_FILETYPE			"FileType"
#define DEF_FILETYPE			""
#define INI_MIME_TYPE			"MimeType"
#define DEF_MIME_TYPE			""

#define INI_FILE_TYPE_ASSOCIATIONS	"File Type Associations"

#define FILE_ASSOC_SEPARATORS		",;"

/* allow duplicate sessions for same connection description */
#define INI_ALLOWDUPLICATESESSIONS	"AllowDuplicateSessions"
#define DEF_ALLOWDUPLICATESESSIONS	FALSE


/* 
 * on a SunRay determine if we should be allowed to run if 
 * no hostid could be determined
 */
#define INI_ALLOWNOSUNRAYID		"AllowNoSunRayID"
#define DEF_ALLOWNOSUNRAYID		FALSE
#define INI_SUNRAYCLIENTNAME            "SunRayClientName"
#define DEF_SUNRAYCLIENTNAME            "Sunray-"

/* 
 * USB speechmike (VDSPMIKE.DLL)
 */
#define INI_USBSPEECHMIKEDEV		"USBSpeechMike"
#define DEF_USBSPEECHMIKEDEV		"/dev/input/event5"
#define INI_USBSPEECHMIKETYPE		"USBSpeechMikeType"
#define DEF_USBSPEECHMIKETYPE		"1883,2512,149a"
#define INI_USBSPEECHMIKEDSP		"USBSpeechMikeDSP"
#define DEF_USBSPEECHMIKEDSP		"/dev/dsp1"
#define INI_USBSPEECHMIKEUSESDEVHID     "USBSpeechMikeUsesDevHID"
#define DEF_USBSPEECHMIKEUSESDEVHID     FALSE

/* Pre-answer question about continuing with PDA without creating lockfile */
#define INI_NO_LOCKFILE		"ContinueWithoutPDALockFile"
/* Default is neither TRUE nor FALSE to allow detection of entry */
#define DEF_NO_LOCKFILE		2   

/* Should be in logapi.h */

#define DEF_LOG_TAIL_SIZE		(64 * 1024)

/* To fix priority problem when deciding which window display settings to use
   Give which ever is set the lowest priority */
#define INI_WINSETTING		"WinSetting"
#define WINSETTING_FULL		0
#define WINSETTING_FIXED	1
#define WINSETTING_PERCENT	2
#define INI_WINSETTING_FULL	"0"
#define INI_WINSETTING_FIXED	"1"
#define INI_WINSETTING_PERCENT	"2"
#define WINSETTING_OPTIONCOUNT   3

/* Should be logapi.h */

int LogMergeFlags (int type, ULONG *value, char *strPtr);

/* Should be in logflags.h */

/* DEBUG: trace option classes */

#if defined(DEBUG) && !defined(NEW_LOGGING) // Limit numeric defines to DEBUG builds until new logging is ported 
#define TC_NCS         0x00100000
#define TC_LIC 	       0x00400000	/* old Unix FLEXlm licensing */
#define TC_GA          0x02000000       /* GA host-specific routines */
#define TC_CSM         TC_NCS
#endif

#define TT_OBSCURATION 0x00100000       /* Obscured area chasing code in GA. */

#define PD__COUNT PDxCOUNT
#define VD__COUNT VDxCOUNT
#define DLL__OPEN  DLLxOPEN
#define DLL__CLOSE  DLLxCLOSE
#define DLL__UNLOAD  DLLxUNLOAD

#define CALLBACK
typedef LPVOID PWFEDEFAULTMODE, PLOADBALANCE, PENCRYPTIONINIT;
typedef PCHAR LPCSTR, LPTSTR;
typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);
#define HBRUSH HANDLE

/* define the engine polling procedure(?) */
#define PFNWFENGPOLL PFNICAENGPOLL

/* copied from win32/platsys.h - 1st needed for audio */
#define HCTXGLOBAL                      HGLOBAL

#define LOWORD(x)	(x & 0xffff)
#define HIWORD(x)	((x >> 16) & 0xffff)
#define LOBYTE(x)	(x & 0xff)
#define HIBYTE(x)	((x >> 8) & 0xff)

/* copied from wingdi.h. Needed for building GAI */
typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD, *LPRGBQUAD;

/* see mmsystem.h */
typedef UINT MMRESULT;
typedef struct timecaps_tag {
    UINT    wPeriodMin;     /* minimum period supported  */
    UINT    wPeriodMax;     /* maximum period supported  */
} TIMECAPS, *PTIMECAPS;
#define TIMERR_NOERROR        (0) 

#define INFINITE (~(UINT32)0)
#define MAXDWORD 0xFFFFFFFF

#define MAKELONG(a, b) ( (((UINT32)(a)) & 0x0000ffff) | (((UINT32)(b)) << 16) )

#define INVALID_HANDLE_VALUE (HANDLE)-1



#ifdef DEBUG
void    OutputDebugString(const char* string);
#else
#define OutputDebugString(s)
#endif
BOOLEAN GetProgramFileName(LPTSTR lpFilename, DWORD nSize);

/*
 *----------------------------------------------------------------------------
 *  UNIFICATION : End                     
 *----------------------------------------------------------------------------
 */

/* Ugly stuff to keep jmorecfg.h happy. */

#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT

/* Ugly MS Windows stuff. */

#define TEXT(string)    string
#define _T(string)      string

/* How to get the browser timeout. */

#ifdef NCS
#define BROWSER_TIMEOUT TdData.BrowserTimeout
#else
#define BROWSER_TIMEOUT G_ReadTimeout
#endif

/* Extra global variables that are available because most modules are built
 * into the main client program.
 */

extern struct _TD_PD TdData;

#endif /* _UNIX_PLATFIX_H_ */
