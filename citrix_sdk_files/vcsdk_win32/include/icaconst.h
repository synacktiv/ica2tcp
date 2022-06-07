/***************************************************************************
*
*  ICACONST.H
*
*  This module contains constants used by the ICA Client.
*
*  Copyright 1998-2015 Citrix Systems, Inc. 
*
****************************************************************************/

#ifndef _ICACONST_H_
#define _ICACONST_H_

#pragma once

/*
 *  String lengths and invalid character defines
 */


#ifdef  VERSION1_LABEL
#define LABEL_LENGTH            12
#define LABEL_INVALID_CHARACTERS                "=<>|\"@&()^;,\\/-"
#define LABEL_INVALID_CHARACTERS_SPACED         "= < > | \" @ & ( ) ^ ; ,\\ / -"
#endif


#define DOSFILENAME_LENGTH      12
#define DESCRIPTION_INVALID_CHARACTERS          "\\/:*?\"<>|,.()[];"
#define DESCRIPTION_INVALID_CHARACTERS_SPACED   "\\ / : * ? \" < > | , . ( ) [ ] ;"
#define DESCRIPTION_LENGTH      40 
#define FRIENDLYNAME_LENGTH     256
#define VERYLARGENAME_LENGTH    512
#define PROGRAMGROUP_LENGTH     30
#define NAMEENUMERATOR_LENGTH   13
#define NAMERESOLVER_LENGTH     13
#define TRANSPORTNAME_LENGTH    40
#define ENCRYPTION_LEVEL_NAME_LENGTH 60
#define EMULATIONNAME_LENGTH    40
#define ENCRYPTIONLEVEL_LENGTH  37


#ifdef  LAUNCHREFERENCE_LENGTH
#undef  LAUNCHREFERENCE_LENGTH
#endif
#define LAUNCHREFERENCE_LENGTH       256

#ifdef  LAUNCHER_LENGTH
#undef  LAUNCHER_LENGTH
#endif
#define LAUNCHER_LENGTH       256

#ifdef  VERSION_LENGTH
#undef  VERSION_LENGTH
#endif
#define VERSION_LENGTH         64


#ifdef  CLIENTNAME_LENGTH
#undef  CLIENTNAME_LENGTH
#endif
#define CLIENTNAME_LENGTH       20                      /* from ica30.h */

/* OUR client serial number length (which is less than max) */
#ifdef  CLIENTSN_LENGTH
#undef  CLIENTSN_LENGTH
#endif
#define CLIENTSN_LENGTH         20

#ifdef  DOMAIN_LENGTH
#undef  DOMAIN_LENGTH
#endif
#define DOMAIN_LENGTH           256                      /* from ica30.h */

#ifdef  USERNAME_LENGTH
#undef  USERNAME_LENGTH
#endif
#define USERNAME_LENGTH         256                      /* from ica30.h */

#ifdef  PASSWORD_LENGTH
#undef  PASSWORD_LENGTH
#endif
#define PASSWORD_LENGTH         256                      /* from ica30.h */

#ifdef  DIRECTORY_LENGTH
#undef  DIRECTORY_LENGTH
#endif
#define DIRECTORY_LENGTH        256                     /* from ica30.h */

#ifdef  MAXOID_LENGTH
#undef  MAXOID_LENGTH
#endif
#define MAXOID_LENGTH       256                      
/* Some earlier than MF1.8/SP2 have a lower limit for Unicode strings. */

#define OLD_DOMAIN_LENGTH        17
#define OLD_USERNAME_LENGTH      20
#define OLD_PASSWORD_LENGTH      14

#ifdef  INITIALPROGRAM_LENGTH
#undef  INITIALPROGRAM_LENGTH
#endif
#define INITIALPROGRAM_LENGTH   256                     /* from ica30.h */

#define ENCRYPTEDPASSWORD_LENGTH   (2*PASSWORD_LENGTH+4+8) /* 4 + 2X + pad */
#define ADDRESS_LENGTH          64   /* should be the same as MAX_BR_NAME-1 */

/* ferdinandoc: fix for long TAPI modem names */

#ifdef WIN32
#define DEVICENAME_LENGTH       70
#else
#define DEVICENAME_LENGTH       59
#endif

#define BAUD_LENGTH             7
#define MODEM_LENGTH            40
#define MODEMINIT_LENGTH        12
#define MODEMINITSTRING_LENGTH  80
#define KEYBOARDLAYOUT_LENGTH   40
#define KEYBOARDTYPE_LENGTH     40
#define HOTKEYNAME_LENGTH       20

#define DIALINGLOCATION_LENGTH  128
#define DIALINGPREFIX_LENGTH    128
#define DIALPROPENTRY_LENGTH    32
#define DIALCOUNTRY_LENGTH      64

#define XMLADDRESSRESTYPE_LENGTH 30

#define MAXPATH 260
#define FILEPATH_LENGTH         MAXPATH

#define SSL_DATA_SIZE							256

#define MAX_ERRORMESSAGE    288

#ifdef MAX_INI_LINE
#undef MAX_INI_LINE
#endif

#define MAX_INI_LINE    270

#define MAX_INI_FILE_PARAMETER 256  /* maximum parameter length */

#define MAX_KEYNAME_SIZE 2048 /* max length for a registry key name or value */

#define MAXINILINE_LENGTH   MAX_INI_LINE

typedef CHAR MAXINILINE[ MAXINILINE_LENGTH+1 ];

/* max count of addresses in server address list
 * must be multiple of 5, because number of servers
 * in a group is a group is 5
 */
#define MAX_BROWSERADDRESSLIST      15

/*
 *  Used for the business recovery feature added to server list
 *
 *  THIS NEEDS TO BE FIXED
 */

#define VALID_ADDRESS 0
#define INVALID_ADDRESS 1
#define SEPERATOR_ADDRESS 2

#define SEPERATOR "-----"

#define RESOLVEWITHDNS "!"

#define ERROR_ADDRESS_IS_SEPERATOR 5

#define SERVERS_IN_GROUP        5

#define SERVER_GROUP1     "Primary"
#define SERVER_GROUP2     "Backup 1"
#define SERVER_GROUP3     "Backup 2"
#define SERVER_GROUP4     "Backup 3"
#define SERVER_GROUP5     "Backup 4"

#define INI_SERVER_GROUP_NAME   "ServerGroupName"

#define MAX_SERVER_GROUP_NAME_SIZE      20

#define CTX_APP_DELIMITER       '#'
#define CTX_APP_DELIMITER_LFN   '"'


/*
 *  The ICA protocol limits cookies to 64 KB, but our ludicrous IPC mechanism
 *  forces me to pick a much smaller size.
 *
 *  Note: Keep this in sync with DEF_COOKIE_MAX_SIZE in icaini.h
 */
#define MAX_COOKIE_SIZE     128

/* 
cpr 233501 
The name of the lock file to prevent pn ( and possible other exes) 
from running when a reboot requirement is pending
*/
#define REBOOT_LOCK_FILE_NAME  "Reboot.lck"

/*
 * TransparentKeyboardPassthrough (aka Keyboard Mapping)
 */
#define TPKEYPASSTHRU_LENGTH    64

#endif /* _ICACONST_H_ */
