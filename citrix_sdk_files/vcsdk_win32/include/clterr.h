/* Copyright 2002-2011 Citrix Systems, Inc. */
/***************************************************************************
*
*  CLTERR.H
*
*  This module contains the client error and status codes.
*
*  Version: $Id: //icaclientcore/main/src/inc/clterr.h#19 
*
*  Copyright 1995-2015 Citrix Systems, Inc. All Rights Reserved.
*
***************************************************************************/
#ifndef _CLTERR_H_
#define _CLTERR_H_

#include "sslerr.h"
/*=============================================================================
 ==  Client Status and Error codes
 ============================================================================*/

/*
 *  Exit code delimiters
 */
#define CLIENT_STATUS_SUCCESS                        0
#define CLIENT_ERROR                              1000

/*
 *  WdPoll status codes (1-254)
 */
#define CLIENT_STATUS_CONNECTED                      1
#define CLIENT_STATUS_CONNECTION_BROKEN_HOST         2
#define CLIENT_STATUS_CONNECTION_BROKEN_CLIENT       3
#define CLIENT_STATUS_MODEM_INIT                     4
#define CLIENT_STATUS_MODEM_DIALING                  5
#define CLIENT_STATUS_MODEM_WAITING                  6
#define CLIENT_STATUS_MODEM_NO_RESPONSE              7
#define CLIENT_STATUS_MODEM_ERROR                    8
#define CLIENT_STATUS_MODEM_NO_DIAL_TONE             9
#define CLIENT_STATUS_MODEM_REDIALING               10
#define CLIENT_STATUS_MODEM_VOICE                   11
#define CLIENT_STATUS_MODEM_BUSY                    12
#define CLIENT_STATUS_MODEM_TERMINATE               13
#define CLIENT_STATUS_MODEM_TIMEOUT                 14
#define CLIENT_STATUS_TTY_CONNECTED                 15
#define CLIENT_STATUS_MODEM_OUT_OF_RETRIES          16
/* Available                                        17  */
#define CLIENT_STATUS_KILL_FOCUS                    18
#define CLIENT_STATUS_REBOOT                        19
#define CLIENT_STATUS_CTRLBREAK                     20
#define CLIENT_STATUS_NO_DATA                       21
#define CLIENT_STATUS_MENUKEY                       22
#define CLIENT_STATUS_QUERY_CLOSE                   23   /* (WIN) wait for response */
#define CLIENT_STATUS_LOADING_STACK                 24
#define CLIENT_STATUS_CONNECTING                    25
#define CLIENT_STATUS_SEND_START                    26
#define CLIENT_STATUS_SEND_STOP                     27
#define CLIENT_STATUS_RECEIVE_START                 28
#define CLIENT_STATUS_RECEIVE_STOP                  29
#define CLIENT_STATUS_ERROR_RETRY                   30
#define CLIENT_STATUS_BEEPED                        31
#define CLIENT_STATUS_DELETE_CONNECT_DIALOG         32
#define CLIENT_STATUS_DISCONNECTING                 33
#define CLIENT_STATUS_QUERY_ACCESS                  34   /* (WIN) wait for response */
#define CLIENT_STATUS_PAPP_LAUNCH_SUCCESS           35
#define CLIENT_STATUS_PAPP_LAUNCH_FAILURE           36
#define CLIENT_STATUS_PASSTHRU_LOGOFF_FAILURE       37
#define CLIENT_STATUS_FINISHED_LOGIN                38
#define CLIENT_STATUS_STARTING_LOGIN                39

#define CLIENT_STATUS_NEWDESKTOPINFO                40 /* new desktop info is available */
#define CLIENT_STATUS_NEWGRAPHPARAMS                41 /* new graphics parameters available */
#define CLIENT_STATUS_MAINWNDHIDE                   42 /* UI should hide parent window if applicable */
#define CLIENT_STATUS_MAINWNDSHOW                   43 /* UI should show parent window if applicable */
#define CLIENT_STATUS_TDEVENTDRIVEN                 44 /* The Engine should switch to fastpoll detection */
#define CLIENT_STATUS_ZL_ENABLED                    45 /* ZL keyboard mode was enabled. The UI might want to display status message */
#define CLIENT_STATUS_ZL_DISABLED                   46 /* ZL keyboard mode was disabled. The UI might want to display status message */
#define CLIENT_STATUS_ZL_ENABLED_HOTKEY             47 /* ZL keyboard mode was enabled using Hotkey. The UI might want to display status message */
#define CLIENT_STATUS_ZL_DISABLED_HOTKEY            48 /* ZL keyboard mode was disabled using Hotkey. The UI might want to display status message */
#define CLIENT_STATUS_SWAP_KEYMAPS                  49 /* Unix keyboard handling - use unicode tables */
#define CLIENT_STATUS_MAINWND_RESTORE_CAPTION       50 /* Add caption to the main window */
#define CLIENT_STATUS_USE_CLIENT_CODEPAGE           51 /* If no server codepage is to be had, get the client one */
#define CLIENT_STATUS_SEAMLESS_INIT_COMPLETE        52 /* The seamless subsystem has initialized*/
#define CLIENT_STATUS_INITIALIZATION_COMPLETE       53 /* The client has finished its initialisation tasks */

#define CLIENT_STATUS_REDRAW_CONTROL_WINDOW         54 /* notify the control window to redraw (ICO) */
#define CLIENT_STATUS_VIRTUAL_CHANNEL_DATA_READY    55 /* virtual channel data has arrived (ICO)    */
#define CLIENT_STATUS_WINDOW_DISPLAYED              56 /* client window is displayed (ICO)          */
#define CLIENT_STATUS_WINDOW_HIDDEN                 57 /* client window is hidden (ICO)             */
#define CLIENT_STATUS_SPEEDBROWSE_STARTED           58 /*SpeedBrowse started*/
#define CLIENT_STATUS_ICO_TAB                       59 /* TAB key for ICO */

#define CLIENT_STATUS_QUERY_AUDIO_ACCESS            60   /* (WIN) wait for response */

#define CLIENT_STATUS_RESOLVING_APP                 61
#define CLIENT_STATUS_CGP_DISCONNECT                63 
#define CLIENT_STATUS_CGP_RECONNECT                 64 
#define CLIENT_STATUS_QUERY_PDA_ACCESS              65   /* (WIN) wait for response */
#define CLIENT_STATUS_PDA_CONTENTION                66
#define CLIENT_STATUS_QUERY_TWN_ACCESS              67   /* (WIN) wait for response */
#define CLIENT_STATUS_SERVER_NOT_TRUSTED            68 /* Client Selective Trust has aborted a connection to an untrusted server */
#define CLIENT_STATUS_CGP_WARNING                   69
#define CLIENT_STATUS_CGP_UNWARN                    70
#define CLIENT_STATUS_CGP_DISCONNECT_WARN           71

#define CLIENT_STATUS_LOGIN_SUCCESS                 72
#define CLIENT_STATUS_LOCK_WORKSTATION              73
#define CLIENT_STATUS_RETRIEVE_ICA_AUTH_DECISION    74

#define CLIENT_STATUS_TWI_CREATED_APP_WINDOW        74
#define CLIENT_STATUS_VDTUI_ACTIVE	            75

#define CLIENT_STATUS_DELETE_ACR                    76
#define CLIENT_STATUS_QUERY_PNP_ACCESS              77   /* (WIN) wait for response */

#define CLIENT_STATUS_TWI_CREATED_FIRST_WINDOW      78   /* 1st seamless window has been created, this is usually the host statUI window */
#define CLIENT_STATUS_TWI_CREATED_FIRST_APP_WINDOW  79   /* 1st seamless application window has been created, used to dismiss client statUI window */
#define CLIENT_STATUS_DIFF_LOCAL_DRIVE_WARNING      80	 /* Folder has been mapped to different drive */
#define CLIENT_STATUS_SERVER_NOCFR_SUPPORT		    81	 /* Server doesn't support CFR */

/* Range of 200-299 is reserved for mutli-threaded client statuses */

/* Range of 600-899 is reserved for specific client implementations */
#define CLIENT_STATUS_RESERVED_6XX_8XX              600

#define CLIENT_BASE_HOTKEY                          900
/*
 * Following CLIENT_STATUS_HOTKEYxxx macros will be removed.
 * We may want to reserve 900-949 for hotkeys which should start from CLIENT_BASE_HOTKEY
 * I will remove them after Mac\Unix change their code.
 */
#define CLIENT_STATUS_HOTKEY1                       900 /* All       */
#define CLIENT_STATUS_HOTKEY2                       901 /* All       */
#define CLIENT_STATUS_HOTKEY3                       902 /* Win only  */
#define CLIENT_STATUS_HOTKEY4                       903 /* Win only  */
#define CLIENT_STATUS_HOTKEY5                       904 /* Win only  */
#define CLIENT_STATUS_HOTKEY6                       905 /* Win only  */
#define CLIENT_STATUS_HOTKEY7                       906 /* Win only  */
#define CLIENT_STATUS_HOTKEY8                       907 /* Win only  */
#define CLIENT_STATUS_HOTKEY9                       908 /* Win only  */
#define CLIENT_STATUS_HOTKEY10                      909 /* Win only  */
#define CLIENT_STATUS_HOTKEY11                      910 /* Win only  */


/*      Japanese hotkey definitions for Macintosh */

#define CLIENT_STATUS_HOTKEYJPN1                    914 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN2                    915 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN3                    916 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN4                    917 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN5                    918 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN6                    919 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN7                    920 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN8                    921 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN9                    922 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN10                   923 /* Mac only */
#define CLIENT_STATUS_HOTKEYJPN11                   924 /* Mac only */

/*
 *  General errors
 */
#define CLIENT_ERROR                              1000
#define CLIENT_ERROR_NO_MEMORY                    1001
#define CLIENT_ERROR_BAD_OVERLAY                  1002
#define CLIENT_ERROR_BAD_PROCINDEX                1003
#define CLIENT_ERROR_BUFFER_TOO_SMALL             1004
#define CLIENT_ERROR_CORRUPT_ALLOC_HEADER         1005
#define CLIENT_ERROR_CORRUPT_ALLOC_TRAILER        1006
#define CLIENT_ERROR_CORRUPT_FREE_HEADER          1007
#define CLIENT_ERROR_CORRUPT_SEG_HEADER           1008
#define CLIENT_ERROR_MEM_ALREADY_FREE             1009
#define CLIENT_ERROR_MEM_TYPE_MISMATCH            1010
#define CLIENT_ERROR_NULL_MEM_POINTER             1011
#define CLIENT_ERROR_IO_PENDING                   1012
#define CLIENT_ERROR_INVALID_BUFFER_SIZE          1013
#define CLIENT_ERROR_INVALID_MODE                 1014
#define CLIENT_ERROR_NOT_OPEN                     1015
#define CLIENT_ERROR_NO_OUTBUF                    1016
#define CLIENT_ERROR_DLL_PROCEDURE_NOT_FOUND      1017
#define CLIENT_ERROR_DLL_LARGER_THAN_256K         1018
#define CLIENT_ERROR_DLL_BAD_EXEHEADER            1019
#define CLIENT_ERROR_OPEN_FAILED                  1020
#define CLIENT_ERROR_INVALID_HANDLE               1021
#define CLIENT_ERROR_VD_NOT_FOUND                 1022
#define CLIENT_ERROR_WD_NAME_NOT_FOUND            1023
#define CLIENT_ERROR_PD_NAME_NOT_FOUND            1024
#define CLIENT_ERROR_THINWIRE_OUTOFSYNC           1025
#define CLIENT_ERROR_NO_MOUSE                     1026
#define CLIENT_ERROR_INVALID_CALL                 1027
#define CLIENT_ERROR_QUEUE_FULL                   1028
#define CLIENT_ERROR_INVALID_DLL_LOAD             1029
#define CLIENT_ERROR_PD_ERROR                     1030      /* call GetLastError */
#define CLIENT_ERROR_VD_ERROR                     1031      /* call GetLastError */
#define CLIENT_ERROR_ALREADY_OPEN                 1032
#define CLIENT_ERROR_PORT_NOT_AVAILABLE           1033
#define CLIENT_ERROR_IO_ERROR                     1034
#define CLIENT_ERROR_THINWIRE_CACHE_ERROR         1035
#define CLIENT_ERROR_BAD_FILE                     1036
#define CLIENT_ERROR_CONFIG_NOT_FOUND             1037
#define CLIENT_ERROR_SERVER_FILE_NOT_FOUND        1038
#define CLIENT_ERROR_PROTOCOL_FILE_NOT_FOUND      1039
#define CLIENT_ERROR_MODEM_FILE_NOT_FOUND         1040
#define CLIENT_ERROR_LAN_NOT_AVAILABLE            1041
#define CLIENT_ERROR_PD_TRANSPORT_UNAVAILABLE     1042      /* unavail on open   */
#define CLIENT_ERROR_INVALID_PARAMETER            1043
#define CLIENT_ERROR_WD_NOT_LOADED                1044
#define CLIENT_ERROR_NOT_CONNECTED                1045
#define CLIENT_ERROR_VD_NOT_LOADED                1046
#define CLIENT_ERROR_ALREADY_CONNECTED            1047
#define CLIENT_ERROR_WFENGINE_NOT_FOUND           1048
#define CLIENT_ERROR_ENTRY_NOT_FOUND              1049
#define CLIENT_ERROR_PD_ENTRY_NOT_FOUND           1050
#define CLIENT_ERROR_WD_ENTRY_NOT_FOUND           1051
#define CLIENT_ERROR_PD_SECTION_NOT_FOUND         1052
#define CLIENT_ERROR_WD_SECTION_NOT_FOUND         1053
#define CLIENT_ERROR_DEVICE_SECTION_NOT_FOUND     1054
#define CLIENT_ERROR_VD_SECTION_NOT_FOUND         1055
#define CLIENT_ERROR_VD_NAME_NOT_FOUND            1056
#define CLIENT_ERROR_SERVER_CONFIG_NOT_FOUND      1057
#define CLIENT_ERROR_CONFIG_CONFIG_NOT_FOUND      1058
#define CLIENT_ERROR_HOTKEY_SHIFTSTATES_NOT_FOUND 1059
#define CLIENT_ERROR_HOTKEY_KEYS_NOT_FOUND        1060
#define CLIENT_ERROR_KEYBOARDLAYOUT_NOT_FOUND     1061
#define CLIENT_ERROR_UI_ENGINE_VER_MISMATCH       1062
#define CLIENT_ERROR_IPC_TIMEOUT                  1063
#define CLIENT_ERROR_UNENCRYPT_RECEIVED           1064
#define CLIENT_ERROR_NENUM_NOT_DEFINED            1065
#define CLIENT_ERROR_NENUM_NO_SERVERS_FOUND       1066
#define CLIENT_ERROR_NENUM_NETWORK_ERROR          1067
#define CLIENT_ERROR_NENUM_WINDOWS95_NOT_SUPPORTED 1068
#define CLIENT_ERROR_CONNECTION_TIMEOUT           1069
#define CLIENT_ERROR_DRIVER_UNSUPPORTED           1070
#define CLIENT_ERROR_NO_MASTER_BROWSER            1071
#define CLIENT_ERROR_TRANSPORT_NOT_AVAILABLE      1072
#define CLIENT_ERROR_NO_NAME_RESOLVER             1073
#define CLIENT_ERROR_SEVEN_BIT_DATA_PATH          1074
#define CLIENT_ERROR_WIN16_WFENGINE_ALREADY_RUNNING 1075
#define CLIENT_ERROR_HOST_NOT_SECURED             1076
#define CLIENT_ERROR_ENCRYPT_UNSUPPORT_BYCLIENT   1077
#define CLIENT_ERROR_ENCRYPT_UNSUPPORT_BYHOST     1078
#define CLIENT_ERROR_ENCRYPT_LEVEL_INCORRECTUSE   1079
#define CLIENT_ERROR_BAD_OVERRIDES                1080
#define CLIENT_ERROR_MISSING_CONNECTION_SECTION   1081
#define CLIENT_ERROR_BAD_COMBINE_ENTRIES          1082
#define CLIENT_ERROR_MISSING_WFCLIENT_SECTION     1083
#define CLIENT_ERROR_BAD_ENTRY_INSERTION          1084
#define CLIENT_ERROR_BAD_HEADER_INSERTION         1085
#define CLIENT_ERROR_BAD_CONCAT_SECTIONS          1086
#define CLIENT_ERROR_MISSING_SECTION              1087
#define CLIENT_ERROR_DUPLICATE_SECTIONS           1088
#define CLIENT_ERROR_DRIVER_BAD_CONFIG            1089
#define CLIENT_ERROR_MISMATCHED_ENCRYPTION        1090
#define CLIENT_ERROR_TAPI_NO_INIT                 1091
#define CLIENT_ERROR_TAPI_VER                     1092
#define CLIENT_ERROR_TAPI_NO_LINES                1093
#define CLIENT_ERROR_TAPI_LINE_NO_EXIST           1094
#define CLIENT_ERROR_TAPI_NEGOTIATE_LINE          1095
#define CLIENT_ERROR_TAPI_CALL_NOT_MADE           1096
#define CLIENT_ERROR_NO_MEMORY_LOAD_AUDIO_VESA    1097
#define CLIENT_ERROR_NO_MEMORY_LOAD_AUDIO         1098
#define CLIENT_ERROR_NO_MEMORY_LOAD_VESA          1099
#define CLIENT_ERROR_NO_MEMORY_LOAD               1100
#define CLIENT_ERROR_IPX_CONNECT_TIMEOUT          1101
#define CLIENT_ERROR_NETWORK_ENUM_FAIL            1102
#define CLIENT_ERROR_PARTIAL_DISCONNECT           1103
#define CLIENT_ERROR_CRITICAL_SECTION_IN_USE      1104
#define CLIENT_ERROR_DISK_FULL                    1105
#define CLIENT_ERROR_DISK_WRITE_PROTECTED         1106
#define CLIENT_ERROR_DLL_NOT_FOUND                1107
#define CLIENT_ERROR_HOST_REQUEST_FAILED          1108
#define CLIENT_ERROR_ENCRYPTION_NOT_SUPPORTED     1109
#define CLIENT_ERROR_CONNECTION_FAILED            1110
#define CLIENT_ERROR_NO_SUCH_SERVER               1111
#define CLIENT_ERROR_OUT_OF_VC                    1112
#define CLIENT_ERROR_POSSIBLE_FILE_CORRUPTION     1113   /* Prompt engine to call into CDM driver to display details */
#define CLIENT_ERROR_INVALID_LICENSE              1114
#define CLIENT_ERROR_REQUIRE_HTC                  1115
#define CLIENT_ERROR_INVALID_SIGNATURE            1116
#define CLIENT_ERROR_THINWIRE_PROTO_CRPT          1117
#define CLIENT_ERROR_NO_APP_AVAILABLE             1118
#define CLIENT_ERROR_REFUSED_BY_EARLY_AUTH        1119
#define CLIENT_ERROR_HOST_NAME_INVALID			  1120
#define CLIENT_ERROR_INVALID_WINDOW_HANDLE        1121
#define CLIENT_ERROR_SELECTIVETRUST_AUTH_DENIED   1122
#define CLIENT_ERROR_SENDMSG_TO_CCMREDIRECTINFO   1123

/* SSL error codes, there are two sets of SSL errors, SDK generated
 * and ICA client generated.
 */
#define CLIENT_ERROR_SSLSDK_OFFSET 1200
#define CLIENT_ERROR_SSL_OFFSET 1300

#define CLIENT_ERROR_SSLSDK_SUCCESS (CLIENT_ERROR_SSLSDK_OFFSET + 0)
#define CLIENT_ERROR_SSLSDK_RENEGOTIATE (CLIENT_ERROR_SSLSDK_OFFSET + 1)
#define CLIENT_ERROR_SSLSDK_HANDSHAKE_COMPLETE (CLIENT_ERROR_SSLSDK_OFFSET + 2)
#define CLIENT_ERROR_SSLSDK_UNKNOWN_ERROR (CLIENT_ERROR_SSLSDK_OFFSET + 3)
#define CLIENT_ERROR_SSLSDK_NETWORK_ERROR (CLIENT_ERROR_SSLSDK_OFFSET + 4)
#define CLIENT_ERROR_SSLSDK_INTERNAL_ERROR (CLIENT_ERROR_SSLSDK_OFFSET + 5)
#define CLIENT_ERROR_SSLSDK_LIBRARY_NOT_AVAILABLE (CLIENT_ERROR_SSLSDK_OFFSET + 6)
#define CLIENT_ERROR_SSLSDK_NO_ENTRY_POINT_DEFINED (CLIENT_ERROR_SSLSDK_OFFSET + 7)
#define CLIENT_ERROR_SSLSDK_INITIALISATION_FAILED (CLIENT_ERROR_SSLSDK_OFFSET + 8)
#define CLIENT_ERROR_SSLSDK_OUT_OF_MEMORY (CLIENT_ERROR_SSLSDK_OFFSET + 9)
#define CLIENT_ERROR_SSLSDK_CREDENTIALS_NOT_FOUND (CLIENT_ERROR_SSLSDK_OFFSET + 10)
#define CLIENT_ERROR_SSLSDK_CREDENTIALS_NOT_RECOGNISED (CLIENT_ERROR_SSLSDK_OFFSET + 11)
#define CLIENT_ERROR_SSLSDK_CREDENTIALS_ACCESS_DENIED (CLIENT_ERROR_SSLSDK_OFFSET + 12)
#define CLIENT_ERROR_SSLSDK_SSL_NOT_AVAILABLE (CLIENT_ERROR_SSLSDK_OFFSET + 13)
#define CLIENT_ERROR_SSLSDK_INSUFFICIENT_SECURITY_AVAILABLE (CLIENT_ERROR_SSLSDK_OFFSET + 14)
#define CLIENT_ERROR_SSLSDK_SECURITY_CONTEXT_INVALID (CLIENT_ERROR_SSLSDK_OFFSET + 15)
#define CLIENT_ERROR_SSLSDK_BAD_SSL_PACKET (CLIENT_ERROR_SSLSDK_OFFSET + 16)
#define CLIENT_ERROR_SSLSDK_BAD_SOCKS5_PACKET (CLIENT_ERROR_SSLSDK_OFFSET + 17)
#define CLIENT_ERROR_SSLSDK_MESSAGE_ALTERED (CLIENT_ERROR_SSLSDK_OFFSET + 18)
#define CLIENT_ERROR_SSLSDK_MESSAGE_OUT_OF_SEQUENCE (CLIENT_ERROR_SSLSDK_OFFSET + 19)
#define CLIENT_ERROR_SSLSDK_MESSAGE_INCOMPLETE (CLIENT_ERROR_SSLSDK_OFFSET + 20)
#define CLIENT_ERROR_SSLSDK_BAD_SOCKS5_METHOD_RESPONSE (CLIENT_ERROR_SSLSDK_OFFSET + 21)
#define CLIENT_ERROR_SSLSDK_BAD_SOCKS5_CONNECT_RESPONSE (CLIENT_ERROR_SSLSDK_OFFSET + 22)
#define CLIENT_ERROR_SSLSDK_UNSUPPORTED_SOCKS_ADDRESS_TYPE (CLIENT_ERROR_SSLSDK_OFFSET + 23)
#define CLIENT_ERROR_SSLSDK_SEND_AND_TERMINATE (CLIENT_ERROR_SSLSDK_OFFSET + 24)
#define CLIENT_ERROR_SSLSDK_PERFORM_REDIRECTION (CLIENT_ERROR_SSLSDK_OFFSET + 25)
#define CLIENT_ERROR_SSLSDK_UNABLE_TO_OPEN_KEYSTORE (CLIENT_ERROR_SSLSDK_OFFSET + 26)
#define CLIENT_ERROR_SSLSDK_UNABLE_TO_FIND_CERTIFICATE (CLIENT_ERROR_SSLSDK_OFFSET + 27)
#define CLIENT_ERROR_SSLSDK_INVALID_SOCKET_TYPE (CLIENT_ERROR_SSLSDK_OFFSET + 28)
#define CLIENT_ERROR_SSLSDK_SOCKS5_HANDSHAKE_FAILED (CLIENT_ERROR_SSLSDK_OFFSET + 29)
#define CLIENT_ERROR_SSLSDK_BUFFER_TOO_SMALL (CLIENT_ERROR_SSLSDK_OFFSET + 30)
#define CLIENT_ERROR_SSLSDK_INVALID_CONTEXT (CLIENT_ERROR_SSLSDK_OFFSET + 31)
#define CLIENT_ERROR_SSLSDK_BAD_SOCKS5_METHOD_REQUEST (CLIENT_ERROR_SSLSDK_OFFSET + 32)
#define CLIENT_ERROR_SSLSDK_BAD_SOCKS5_CONNECT_REQUEST (CLIENT_ERROR_SSLSDK_OFFSET + 33)
#define CLIENT_ERROR_SSLSDK_SOCKS5_COMMAND_UNSUPPORTED (CLIENT_ERROR_SSLSDK_OFFSET + 34)
#define CLIENT_ERROR_SSLSDK_REDIRECTION_ACCESS_FAILURE (CLIENT_ERROR_SSLSDK_OFFSET + 35)
#define CLIENT_ERROR_SSLSDK_SOCKS5_NETWORK_UNREACHABLE (CLIENT_ERROR_SSLSDK_OFFSET + 36)
#define CLIENT_ERROR_SSLSDK_SOCKS5_HOST_UNREACHABLE (CLIENT_ERROR_SSLSDK_OFFSET + 37)
#define CLIENT_ERROR_SSLSDK_SOCKS5_CONNECTION_REFUSED (CLIENT_ERROR_SSLSDK_OFFSET + 38)
#define CLIENT_ERROR_SSLSDK_SOCKS5_TTL_EXPIRED (CLIENT_ERROR_SSLSDK_OFFSET + 39)
#define CLIENT_ERROR_SSLSDK_HOSTNAME_NOT_FOUND (CLIENT_ERROR_SSLSDK_OFFSET + 40)
#define CLIENT_ERROR_SSLSDK_UNABLE_TO_CREATE_SOCKET (CLIENT_ERROR_SSLSDK_OFFSET + 41)
#define CLIENT_ERROR_SSLSDK_CONNECTION_REFUSED (CLIENT_ERROR_SSLSDK_OFFSET + 42)
#define CLIENT_ERROR_SSLSDK_ALERT_CLOSE_NOTIFY (CLIENT_ERROR_SSLSDK_OFFSET + 43)
#define CLIENT_ERROR_SSLSDK_ALERT_UNEXPECTED_MESSAGE (CLIENT_ERROR_SSLSDK_OFFSET + 44)
#define CLIENT_ERROR_SSLSDK_ALERT_BAD_MAC (CLIENT_ERROR_SSLSDK_OFFSET + 45)
#define CLIENT_ERROR_SSLSDK_ALERT_DECOMPRESSION_FAILED (CLIENT_ERROR_SSLSDK_OFFSET + 46)
#define CLIENT_ERROR_SSLSDK_ALERT_HANDSHAKE_FAILED (CLIENT_ERROR_SSLSDK_OFFSET + 47)
#define CLIENT_ERROR_SSLSDK_ALERT_NO_CERTIFICATE (CLIENT_ERROR_SSLSDK_OFFSET + 48)
#define CLIENT_ERROR_SSLSDK_ALERT_BAD_CERTIFICATE (CLIENT_ERROR_SSLSDK_OFFSET + 49)
#define CLIENT_ERROR_SSLSDK_ALERT_UNSUPPORTED_CERT (CLIENT_ERROR_SSLSDK_OFFSET + 50)
#define CLIENT_ERROR_SSLSDK_ALERT_CERTIFICATE_REVOKED (CLIENT_ERROR_SSLSDK_OFFSET + 51)
#define CLIENT_ERROR_SSLSDK_ALERT_CERTIFICATE_EXPIRED (CLIENT_ERROR_SSLSDK_OFFSET + 52)
#define CLIENT_ERROR_SSLSDK_ALERT_CERTIFICATE_UNKNOWN (CLIENT_ERROR_SSLSDK_OFFSET + 53)
#define CLIENT_ERROR_SSLSDK_ALERT_ILLEGAL_PARAMETER (CLIENT_ERROR_SSLSDK_OFFSET + 54)
#define CLIENT_ERROR_SSLSDK_ALERT_UNKNOWN (CLIENT_ERROR_SSLSDK_OFFSET + 55)
#define CLIENT_ERROR_SSLSDK_UNABLE_TO_SET_VERIFY_PATHS (CLIENT_ERROR_SSLSDK_OFFSET + 56)
#define CLIENT_ERROR_SSLSDK_UNABLE_TO_SET_CERTIFICATE (CLIENT_ERROR_SSLSDK_OFFSET + 57)
#define CLIENT_ERROR_SSLSDK_UNABLE_TO_SET_KEY (CLIENT_ERROR_SSLSDK_OFFSET + 58)
#define CLIENT_ERROR_SSLSDK_INVALID_COMMON_NAME (CLIENT_ERROR_SSLSDK_OFFSET + 59)
#define CLIENT_ERROR_SSLSDK_X509ERR_DEPTH_ZERO_SELF_SIGNED_CERT (CLIENT_ERROR_SSLSDK_OFFSET + 60)
#define CLIENT_ERROR_SSLSDK_X509ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY (CLIENT_ERROR_SSLSDK_OFFSET + 61)
#define CLIENT_ERROR_SSLSDK_X509ERR_UNABLE_TO_GET_ISSUER_CERT (CLIENT_ERROR_SSLSDK_OFFSET + 62)
#define CLIENT_ERROR_SSLSDK_X509ERR_SELF_SIGNED_CERT_IN_CHAIN (CLIENT_ERROR_SSLSDK_OFFSET + 63)
#define CLIENT_ERROR_SSLSDK_X509ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE (CLIENT_ERROR_SSLSDK_OFFSET + 64)
#define CLIENT_ERROR_SSLSDK_X509ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY (CLIENT_ERROR_SSLSDK_OFFSET + 65)
#define CLIENT_ERROR_SSLSDK_X509ERR_CERT_SIGNATURE_FAILURE (CLIENT_ERROR_SSLSDK_OFFSET + 66)
#define CLIENT_ERROR_SSLSDK_X509ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD (CLIENT_ERROR_SSLSDK_OFFSET + 67)
#define CLIENT_ERROR_SSLSDK_X509ERR_CERT_NOT_YET_VALID (CLIENT_ERROR_SSLSDK_OFFSET + 68)
#define CLIENT_ERROR_SSLSDK_X509ERR_ERROR_IN_CERT_NOT_AFTER_FIELD (CLIENT_ERROR_SSLSDK_OFFSET + 69)
#define CLIENT_ERROR_SSLSDK_X509ERR_CERT_HAS_EXPIRED (CLIENT_ERROR_SSLSDK_OFFSET + 70)
#define CLIENT_ERROR_SSLSDK_UNIMPLEMENTED (CLIENT_ERROR_SSLSDK_OFFSET + 71)
#define CLIENT_ERROR_SSLSDK_FAILED_TO_LOAD_ANY_CERTIFICATES (CLIENT_ERROR_SSLSDK_OFFSET + 72)
#define CLIENT_ERROR_SSLSDK_FAILED_TO_LOAD_ALL_CERTIFICATES (CLIENT_ERROR_SSLSDK_OFFSET + 73)
#define CLIENT_ERROR_SSLSDK_AUTHENTICATION_FAILURE (CLIENT_ERROR_SSLSDK_OFFSET + 74)
#define CLIENT_ERROR_SSLSDK_TIMEOUT (CLIENT_ERROR_SSLSDK_OFFSET + 75)
#define CLIENT_ERROR_SSLSDK_X509ERR_REVOKED (CLIENT_ERROR_SSLSDK_OFFSET + 76)
#define CLIENT_ERROR_SSLSDK_X509ERR_REVOCATION_CHECK_FAILED (CLIENT_ERROR_SSLSDK_OFFSET + 77)
#define CLIENT_ERROR_SSLSDK_X509ERR_NO_REVOCATION_PROVIDER (CLIENT_ERROR_SSLSDK_OFFSET + 78)

#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_KEY_LENGTH (CLIENT_ERROR_SSLSDK_OFFSET + 79)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_PATH_LENGTH (CLIENT_ERROR_SSLSDK_OFFSET + 80)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_ENHANCED_KEY (CLIENT_ERROR_SSLSDK_OFFSET + 81)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_NETSCAPE (CLIENT_ERROR_SSLSDK_OFFSET + 82)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_CONSTRAINTS (CLIENT_ERROR_SSLSDK_OFFSET + 83)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_KEY_USAGE (CLIENT_ERROR_SSLSDK_OFFSET + 84)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_CRITICAL_EXTENSION (CLIENT_ERROR_SSLSDK_OFFSET + 85)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_CERT_NAME (CLIENT_ERROR_SSLSDK_OFFSET + 86)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_DATE_NESTING (CLIENT_ERROR_SSLSDK_OFFSET + 87)
#define CLIENT_ERROR_SSLSDK_X509ERR_BAD_CERTIFICATE_CHAIN_LENGTH (CLIENT_ERROR_SSLSDK_OFFSET + 88)
#define CLIENT_ERROR_SSLSDK__LAST (CLIENT_ERROR_SSLSDK_OFFSET + 88)

#define CLIENT_ERROR_SSL_CONNREFUSED        (CLIENT_ERROR_SSL_OFFSET + 0)
#define CLIENT_ERROR_SSL_NETUNREACH         (CLIENT_ERROR_SSL_OFFSET + 1)
#define CLIENT_ERROR_SSL_TIMEDOUT           (CLIENT_ERROR_SSL_OFFSET + 2)
#define CLIENT_ERROR_SSL_ILLEGAL_COMMONNAME (CLIENT_ERROR_SSL_OFFSET + 3)
#define CLIENT_ERROR_SSL_NOT_AVAILABLE      (CLIENT_ERROR_SSL_OFFSET + 4)
#define CLIENT_ERROR_SSL__LAST              (CLIENT_ERROR_SSL_OFFSET + 4)

/* 
 * PROXY error codes
 */
#define CLIENT_ERROR_PROXY_OFFSET           1400

#define CLIENT_ERROR_PROXYCONNERR_HOST_MISSING            (CLIENT_ERROR_PROXY_OFFSET+0)
#define CLIENT_ERROR_PROXYCONNERR_SOCKTYPE_UNSUPPORTED    (CLIENT_ERROR_PROXY_OFFSET+1)
#define CLIENT_ERROR_PROXYCONNERR_ALLOC_FAILURE           (CLIENT_ERROR_PROXY_OFFSET+2)
#define CLIENT_ERROR_PROXYCONNERR_NOT_ACCEPTED            (CLIENT_ERROR_PROXY_OFFSET+3)
#define CLIENT_ERROR_PROXYCONNERR_NO_ANSWER               (CLIENT_ERROR_PROXY_OFFSET+4)
#define CLIENT_ERROR_PROXYCONNERR_NET_UNREACHABLE         (CLIENT_ERROR_PROXY_OFFSET+5)
#define CLIENT_ERROR_PROXYCONNERR_HOST_UNREACHABLE        (CLIENT_ERROR_PROXY_OFFSET+6)
#define CLIENT_ERROR_PROXYCONNERR_HOST_NOT_FOUND          (CLIENT_ERROR_PROXY_OFFSET+7)
#define CLIENT_ERROR_PROXYCONNERR_PROTOCOL_ERROR          (CLIENT_ERROR_PROXY_OFFSET+8)
#define CLIENT_ERROR_PROXYCONNERR_AUTH_FAILED             (CLIENT_ERROR_PROXY_OFFSET+9)
#define CLIENT_ERROR_PROXYCONNERR_UNSUPPORTED_AUTH_SCHEME (CLIENT_ERROR_PROXY_OFFSET+10)
#define CLIENT_ERROR_PROXYCONNERR_PROXY_REFUSED           (CLIENT_ERROR_PROXY_OFFSET+11)
#define CLIENT_ERROR_PROXYCONNERR_PROXY_FAILED            (CLIENT_ERROR_PROXY_OFFSET+12)
#define CLIENT_ERROR_PROXYCONNERR_INTERNAL_ERROR          (CLIENT_ERROR_PROXY_OFFSET+13)
#define CLIENT_ERROR_PROXYDTCTERR_ALLOC_FAILURE           (CLIENT_ERROR_PROXY_OFFSET+14)
#define CLIENT_ERROR_PROXYDTCTERR_NO_BROWSER_SETTINGS     (CLIENT_ERROR_PROXY_OFFSET+15)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_BAD_URL          (CLIENT_ERROR_PROXY_OFFSET+16)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_UNKNOWN_HOST     (CLIENT_ERROR_PROXY_OFFSET+17)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_NO_HTTPLIB       (CLIENT_ERROR_PROXY_OFFSET+18)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_CONNECT_FAILURE  (CLIENT_ERROR_PROXY_OFFSET+19)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_NOTFOUND         (CLIENT_ERROR_PROXY_OFFSET+20)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_GENERAL_FAILURE  (CLIENT_ERROR_PROXY_OFFSET+21)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_EMPTY            (CLIENT_ERROR_PROXY_OFFSET+22)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_NO_SUPPORT       (CLIENT_ERROR_PROXY_OFFSET+23)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_ERROR            (CLIENT_ERROR_PROXY_OFFSET+24)
#define CLIENT_ERROR_PROXYDTCTERR_SCRIPT_BAD_RESULT       (CLIENT_ERROR_PROXY_OFFSET+25)
#define CLIENT_ERROR_PROXYWPADERR_DETECT_ERROR            (CLIENT_ERROR_PROXY_OFFSET+26)
#define CLIENT_ERROR_PROXYWPADERR_DOWNLOAD_ERROR          (CLIENT_ERROR_PROXY_OFFSET+27)
#define CLIENT_ERROR_PROXYWPADERR_DESCRIPT_ERROR          (CLIENT_ERROR_PROXY_OFFSET+28)
#define CLIENT_ERROR_PROXYWPADERR_FILE_ERROR              (CLIENT_ERROR_PROXY_OFFSET+29)
#define CLIENT_ERROR_PROXYDTCTERR_TOO_MANY_REDIRECTIONS   (CLIENT_ERROR_PROXY_OFFSET+30)

#define CLIENT_ERROR_PROXY_ERROR__LAST        (CLIENT_ERROR_PROXY_OFFSET+30)

/*
 * Range of 1600-1899 is reserved for specific client implementations
 */
#define CLIENT_ERROR_RESERVED_16XX_18XX           1600

#define CLIENT_ERROR_SEAMLESS_HOST_AGENT_NOT_READY  2100
#define CLIENT_ERROR_SEAMLESS_HOST_BUSY             2101
#define CLIENT_ERROR_NOT_LOGGED_IN                  2102
#define CLIENT_ERROR_SEAMLESS_CLIENT_BUSY           2103
#define CLIENT_ERROR_SEAMLESS_NOT_COMPATIBLE        2104

/* prototypes for functions to make strings from codes or print such */
void display_ICA_status(char *string, int code);
BOOL status_code_to_message(char *string, int code, char *message);


#define CLIENT_ERROR_PROXY_LIBOBJ_MISSING           2200
#define CLIENT_ERROR_BUFFER_STILL_BUSY              2201 
#define CLIENT_ERROR_VD_STILL_BUSY                  2202
#define CLIENT_ERROR_VD_POLLING_NOT_READY			2203
#define CLIENT_ERROR_FOLDER_MAPPING_FAILED			2204 /* Folder has not been mapped at all */

#define CLIENT_ERROR_LOCKDOWN_STATUS                2300

// socket errors
#define WSAERR_ID_IDS_OFFSET		20000
#define WSABASEERR              	10000
#define ID_WSAHOST_NOT_FOUND       	(WSABASEERR+1001)
#define ID_WSAEINTR 				(WSABASEERR+04)
#define ID_WSAEBADF 				(WSABASEERR+09)
#define ID_WSAEACCES 				(WSABASEERR+13)
#define ID_WSAEFAULT				(WSABASEERR+14)
#define ID_WSAEINVAL				(WSABASEERR+22)
#define ID_WSAEMFILE				(WSABASEERR+24)
#define ID_WSAEWOULDBLOCK			(WSABASEERR+35)
#define ID_WSAEINPROGRESS			(WSABASEERR+36)
#define ID_WSAEALREADY             	(WSABASEERR+37)
#define ID_WSAENOTSOCK             	(WSABASEERR+38)
#define ID_WSAEDESTADDRREQ         	(WSABASEERR+39)
#define ID_WSAEMSGSIZE             	(WSABASEERR+40)
#define ID_WSAEPROTOTYPE           	(WSABASEERR+41)
#define ID_WSAENOPROTOOPT          	(WSABASEERR+42)
#define ID_WSAEPROTONOSUPPORT      	(WSABASEERR+43)
#define ID_WSAESOCKTNOSUPPORT      	(WSABASEERR+44)
#define ID_WSAEOPNOTSUPP           	(WSABASEERR+45)
#define ID_WSAEPFNOSUPPORT         	(WSABASEERR+46)
#define ID_WSAEAFNOSUPPORT         	(WSABASEERR+47)
#define ID_WSAEADDRINUSE           	(WSABASEERR+48)
#define ID_WSAEADDRNOTAVAIL        	(WSABASEERR+49)
#define ID_WSAENETDOWN             	(WSABASEERR+50)
#define ID_WSAENETUNREACH          	(WSABASEERR+51)
#define ID_WSAENETRESET            	(WSABASEERR+52)
#define ID_WSAECONNABORTED         	(WSABASEERR+53)
#define ID_WSAECONNRESET           	(WSABASEERR+54)
#define ID_WSAENOBUFS              	(WSABASEERR+55)
#define ID_WSAEISCONN              	(WSABASEERR+56)
#define ID_WSAENOTCONN             	(WSABASEERR+57)
#define ID_WSAESHUTDOWN            	(WSABASEERR+58)
#define ID_WSAETIMEDOUT            	(WSABASEERR+60)
#define ID_WSAECONNREFUSED         	(WSABASEERR+61)
#define ID_WSAELOOP                	(WSABASEERR+62)
#define ID_WSAENAMETOOLONG         	(WSABASEERR+63)
#define ID_WSAEHOSTDOWN            	(WSABASEERR+64)
#define ID_WSAEHOSTUNREACH         	(WSABASEERR+65)
#define ID_WSAENOTEMPTY            	(WSABASEERR+66)
#define ID_WSAEWOULDBLOCK          	(WSABASEERR+35)
#define ID_WSAEINPROGRESS          	(WSABASEERR+36)
#define ID_WSAEFAULT               	(WSABASEERR+14)
#define ID_WSAEINVAL               	(WSABASEERR+22)
#define ID_WSAEMFILE               	(WSABASEERR+24)
#define ID_WSANOTINITIALISED       	(WSABASEERR+93)
#define ID_WSASYSNOTREADY          	(WSABASEERR+91)
#define ID_WSAVERNOTSUPPORTED      	(WSABASEERR+92)

// keep this in track with the highest socket error number
#define ID_WSAERR__LAST				(WSABASEERR+92)

// Keep this in track with the highest error code id number
#define CLIENT_ERROR__LAST												(CLIENT_ERROR_SSL_OFFSET + 4)

/*
 * Range of 3200-3299 is reserved for multi-threaded client specific errors
 */

// common funtions

/**********************************************************************
*
* Method: BOOL IsDetailedError(UINT iErrorCode)
*
* Purpose: Checks for PD error, Socket errors, SSLSDK errors and ssl errors specific to IcaClient.
* 
* Parameters: Error code ID
*
* Returns: If iErrorCode falls in the range of the above error codes , then it reurns true. else false.
*
* Remarks: If this function is changed make the smae changes in function written in Client_Errors.h.
*
***********************************************************************/
__inline BOOL IsDetailedError(INT iErrorCode)
{
	BOOL ret = FALSE;
	if ((CLIENT_ERROR_PD_ERROR == iErrorCode) || (iErrorCode >= WSABASEERR && iErrorCode <= ID_WSAERR__LAST) ||
		(iErrorCode == ID_WSAHOST_NOT_FOUND) || (iErrorCode >= SSLERROROFFSET && iErrorCode <= SSLERROR_LASTERROR) ||
		(iErrorCode >= SDKERROROFFSET && iErrorCode <= iSSL_STATUS_LASTERROR ))
	{
		ret = TRUE;
	}
	return ret;
}

/**********************************************************************
*
* Method: IsErrorMsgEnabled()
*
* Purpose: Fetches the value of registry Enable_EngineErrMsg
*
* Parameters: None
* Returns: DWORD
*               Returns if the Error Messaging of Engine is enabled (Includes Socket errors currently)
*
* Remarks: If Engine Error Messaging is disabled, we will get generic Protocol driver error message else we will get specific errors
*
***********************************************************************/
static DWORD IsErrorMsgEnabled()
{
	HKEY hKey = NULL;
	WCHAR subKey[] = L"Software\\Policies\\Citrix\\ICA Client\\Engine";
	DWORD regType = REG_DWORD;
	WCHAR ValueName[] = L"Enable_EngineErrMsg";
	DWORD dwErrMsgEnabled = 1;
	REGSAM samDesired = KEY_QUERY_VALUE;
	DWORD pValueSize = sizeof(DWORD);

	if((ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, samDesired, &hKey)) && hKey)
	{
		RegQueryValueExW(hKey, ValueName, NULL, &regType, (LPBYTE)&dwErrMsgEnabled, &pValueSize);
		RegCloseKey(hKey);
	}
	else if ((ERROR_SUCCESS == RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, samDesired, &hKey)) && hKey)
	{
		RegQueryValueExW(hKey, ValueName, NULL, &regType, (LPBYTE)&dwErrMsgEnabled, &pValueSize);
		RegCloseKey(hKey);
	}

	return dwErrMsgEnabled;
}

// Get the value of registry Enable_EngineErrMsg using fn IsErrorMsgEnabled()
static int ErrorMsgEnabled()
{
	static int dwErrorMsgFlag = -1;
	if (dwErrorMsgFlag == -1)
	{
		dwErrorMsgFlag = IsErrorMsgEnabled();
	}
	return dwErrorMsgFlag;
}

#endif // _CLTERR_H_
