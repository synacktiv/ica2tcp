/***************************************************************************
*
*  VDAPI.H
*
*  This module contains external virtual driver API defines and structures
*
*  $Id$
*
*  Copyright 1994-2016 Citrix Systems, Inc. All Rights Reserved.
*
****************************************************************************/

#ifndef __VDAPI_H__
#define __VDAPI_H__

#include <icaconst.h>
#include <dllapi.h>
#include <ica.h>

/*
 *  Index into PD procedure array
 */
#define VDxQUERYINFORMATION      6
#define VDxSETINFORMATION        7
#define VDxCOUNT                 8      /* Number of external procedures. */


/*
 *  VdOpen structure
 */
typedef struct _VDOPEN {
    PDLLLINK            pVdLink;        /* DLLLINK for this VD. */
    PDLLLINK            pWdLink;        /* DLLLINK for the WD. */
    ULONG               ChannelMask;    /* Must be set by VD Open function. */
    PLIBPROCEDURE       reserved2; // TODO This needs removing but will break binary API. Thought needed.
    PLIBPROCEDURE       pfnStatusMsgProc;
    HND                 hICAEng;        /* FIXME - still used by Thinwire. */
} VDOPEN,  *PVDOPEN;


/*
 *  VdInformationClass enum
 *   additional information classes can be defined for a given
 *   VD, as long as the number used is greater than that generated
 *   by this enum list
 */

typedef enum _VDINFOCLASS {
#ifndef unix
    VdLastError,
#endif /* unix */
    VdKillFocus,
    VdSetFocus,
#ifndef unix
    VdMousePosition,
#endif /* unix */
    VdThinWireCache,
    VdWinCEClipboardCheck,    /*  Used by WinCE to check for clipboard changes */
    VdDisableModule,
    VdFlush,
    VdInitWindow,
    VdDestroyWindow,
#ifndef unix
    VdPaint,
#endif /* unix */
    VdThinwireStack,
    VdRealizePaletteFG,        /* inform client to realize it's foreground palette */
    VdRealizePaletteBG,        /* inform client to realize it's background palette */
    VdInactivate,              /* client is about to lose input focus */
#ifndef unix
    VdGetSecurityAccess,       /* cdm security info */
    VdSetSecurityAccess,       /* cdm security info */
    VdGetAudioSecurityAccess,  /* cdm audio security info */
    VdSetAudioSecurityAccess,  /* cdm audio security info */
#endif /* unix */
    VdGetPDASecurityAccess,    /* cdm PDA security info */
    VdSetPDASecurityAccess,    /* cdm PDA security info */
#ifndef unix
    VdGetTWNSecurityAccess,    /* cdm TWN security info */
    VdSetTWNSecurityAccess,    /* cdm TWN security info */
#endif /* unix */
    VdSendLogoff,
    VdCCShutdown,

    VdSeamlessHostCommand,        /* Seamless command call */
    VdSeamlessQueryInformation,   /* Seamless query call */

    VdWindowSwitch,
    VdSetCursor,                  /* Set: New cursor from TW. Data - PHGCURSOR */
    VdSetCursorPos,               /* Set: New cursor position. Data - VPPOINT */

    VdEnableState,                /* Set/Get driver state (enabled/disabled) */

    VdIcaControlCommand,

#ifndef unix
    VdVirtualChannel,             /* Set/Get virtual channel data */
    VdWorkArea,                   /* Set the work area of the application */
#endif /* unix */
    VdSupportHighThroughput,
#ifndef unix
    VdRenderingMode,              /* Set/Get the rendering mode (headless client) */
#endif /* unix */
    VdPauseResume,                /* Pause/Resume commands                        */

    VdRedrawNotify,               /* Overdrawing suppression. */
    VdDisplayCaps,                /* Get/Set display capabilities and/or preferred mode */
    VdICOSeamlessFunctions,       /* Get seamless functions for ICO */
    VdPnP,                        /* Set: Send CLPNP_COMMAND commands inbetween the control VC and the */
                                  /*      implementation VC (such as VIRTUAL_CCM) */

    /* Support for EUEM (Darwin Release For Ohio). This code has been added or
     * changed to support End User Experience Monitoring functionality. Please do
     * not change the code in this section without consulting the EUEM team
     *  Email (at the time of change) "#ENG - Darwin Dev".
     */
    VdEuemStartupTimes,           /* Set: EUEM: pass the startup times of shared sessions to the EUEM VD */

    VdEuemTwCallback,             /* Set: register the EUEM ICA roundtrip callback */
                                  /*      function from the thinwire VC to the EUEM VC */
    /* End of EUEM support section */

    VdResizeHotBitmapCache,       /* Set: Tell thinwire driver to resize the hot bitmap cache */
    VdSetMonitorLayout,           /* Set: Tell thinwire driver to update monitor layout info */
    VdGUSBGainFocus,              /* Set: Tell Generic USB driver that engine has gained keyboard focus */
    VdGUSBLoseFocus,              /* Set: Tell Generic USB driver that engine has lost keyboard focus */

#ifdef UDP_AUDIO
    VdRtpConnectionEstablished,   /* Set: Tell VDCAM that first stage of handhake has occurred; include func to send UDP data */
    VdRtpFinalHandshakeReady,     /* Set: Tell VDCAM that final stage of handhake can now occur */
#endif
    VdDimRequest                  /* Enable/Disable session dim based on SR mode */

} VDINFOCLASS;

/*
 *  VdQueryInformation structure
 */
typedef struct _VDQUERYINFORMATION {
    VDINFOCLASS VdInformationClass;
    LPVOID pVdInformation;
    int VdInformationLength;
    int VdReturnLength;
} VDQUERYINFORMATION, * PVDQUERYINFORMATION;

/*
 *  VdSetInformation structure
 */
typedef struct _VDSETINFORMATION {
    VDINFOCLASS VdInformationClass;
    LPVOID pVdInformation;
    int VdInformationLength;
} VDSETINFORMATION, * PVDSETINFORMATION;

/*
 *  VdLastError
 */
typedef struct _VDLASTERROR {
    int Error;
    char Message[ MAX_ERRORMESSAGE ];
} VDLASTERROR, * PVDLASTERROR;

/*
 * VD Flush
 */
typedef struct _VDFLUSH {
    UCHAR Channel;
    UCHAR Mask;
} VDFLUSH, * PVDFLUSH;

#define  VDFLUSH_MASK_PURGEINPUT    0x01
#define  VDFLUSH_MASK_PURGEOUTPUT   0x02


#endif /* __VDAPI_H__ */



