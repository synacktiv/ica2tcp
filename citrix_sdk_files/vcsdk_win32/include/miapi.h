/***************************************************************************
*
*   MIAPI.H
*
*   Header file for Memory INI APIs
*
*   $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/miapi.h#1 $
*
*   Copyright 1998-2015 Citrix Systems, Inc.
*
****************************************************************************/

#ifndef __MIAPI_H__
#define __MIAPI_H__

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MEMINI_STR_LEN    40
#define MAXLINEBUF           128

typedef int    (PWFCAPI PMISETPROFILEPOINTER)           ( PCHAR );
typedef int    (PWFCAPI PMISETSECTIONPOINTER)           ( PCHAR );
#ifdef UNICODE_PROFILE_FUNCTIONS
typedef int    (PWFCAPI PMIGETPRIVATEPROFILEWIDESTRING) ( PCHAR, PCHAR, PCHAR,
                                                          PWCHAR, int );
#endif
typedef int    (PWFCAPI PMIGETPRIVATEPROFILESTRING)     ( PCHAR, PCHAR, PCHAR,
                                                          PCHAR, int );
typedef int    (PWFCAPI PMIGETPRIVATEPROFILEINT)        ( PCHAR, PCHAR, int );
typedef LONG   (PWFCAPI PMIGETPRIVATEPROFILELONG)       ( PCHAR, PCHAR, LONG );
typedef BOOL   (PWFCAPI PMIGETPRIVATEPROFILEBOOL)       ( PCHAR, PCHAR, BOOL );

typedef int    (PWFCAPI PMISETPRIVATEPROFILESTRING)     ( PCHAR, PCHAR, PCHAR );
typedef int    (PWFCAPI PMIRELEASEPROFILE)( );

typedef int    (PWFCAPI PMISERIALIZE)                   ( PCHAR *, ULONG * );
typedef int    (PWFCAPI PMIDESERIALIZE)                 ( PCHAR, ULONG );
typedef int    (PWFCAPI PMIFREESERIALIZEDATA)           ( PCHAR, ULONG );
typedef int    (PWFCAPI PMIADDEVIDENCE)                 ( LPWSTR, LPSTR );
typedef int    (PWFCAPI PMIAUTHORISECONNECTION)         ( LPSTR );
typedef UINT64 (PWFCAPI PMIGETPRIVATEPROFILEUINT64)     ( PCHAR, PCHAR, UINT64 );

typedef int    (PWFCAPI PMIGETPRIVATEPROFILESTRINGU8)   (PCSTR, PCSTR, PCSTR, PCHAR, int);
typedef int    (PWFCAPI PMIGETPRIVATEPROFILESTRINGU16)  (PCSTR, PCSTR, PCSTR, PWCHAR, int);
typedef int    (PWFCAPI PMISETPRIVATEPROFILESTRINGU16)  (PCSTR, PCSTR, PCWSTR);
typedef int    (PWFCAPI PMIGETLONGCOMMANDLINE)          (PCSTR, PCWSTR, PWCHAR, int);
typedef int    (PWFCAPI PMIGETPRECEDENCEANDICASETTING)(PCHAR, PCHAR, INT *, INT *);
typedef BOOL   (PWFCAPI PINITCRITSECTION)();
/*
 * WARNING: do not add new function pointers to the middle of
 * this structure. Doing so will break the VCSDK. Add new
 * function pointers to the end of the structure.
 */
typedef struct _MEMINITABLE
{
    PMISETPROFILEPOINTER        pmiSetProfilePointer;
    PMISETSECTIONPOINTER        pmiSetSectionPointer;
    PMIGETPRIVATEPROFILESTRING  pmiGetPrivateProfileString;
#ifdef INTERNAL_UTF8
    PMIGETPRIVATEPROFILESTRING  pmiGetPrivateProfileStringInUTF8;
#ifdef UNICODE_PROFILE_FUNCTIONS
    PMIGETPRIVATEPROFILEWIDESTRING  pmiGetPrivateProfileStringInUTF16;
#endif
#endif
    PMIGETPRIVATEPROFILEINT     pmiGetPrivateProfileInt;
    PMIGETPRIVATEPROFILELONG    pmiGetPrivateProfileLong;
    PMIGETPRIVATEPROFILEBOOL    pmiGetPrivateProfileBool;
    PMIRELEASEPROFILE           pmiReleaseProfile;
    PMISERIALIZE                pmiSerialize;
    PMIDESERIALIZE              pmiDeserialize;
    PMIFREESERIALIZEDATA        pmiFreeSerializeData;
    PMISETPRIVATEPROFILESTRING  pmiSetPrivateProfileString;
    PMIADDEVIDENCE              pmiAddEvidence;
    PMIAUTHORISECONNECTION      pmiAuthoriseConnection;
    PMIGETPRIVATEPROFILEUINT64  pmiGetPrivateProfileUINT64;
#if defined(WIN32) && !defined(WINCE)
    PMIGETPRIVATEPROFILESTRINGU8 pmiGetPrivateProfileStringInUTF8;
    PMIGETPRIVATEPROFILESTRINGU16 pmiGetPrivateProfileStringInUTF16;
#endif
    PMISETPRIVATEPROFILESTRINGU16 pmiSetPrivateProfileStringInUTF16;
    PMIGETLONGCOMMANDLINE       pmiGetLongCommandLine;
	PMIGETPRECEDENCEANDICASETTING pmiGetPrecedenceAndIcaSetting;
    PINITCRITSECTION            pmiInitCritSection;
} MEMINITABLE, FAR * PMEMINITABLE, FAR * FAR * PPMEMINITABLE;

/*=============================================================================
==   External functions provided by LoadLibraries()
=============================================================================*/

int WFCAPI MemIniLoad (PPMEMINITABLE );
int WFCAPI MemIniUnload( VOID );

/*=============================================================================
==   API entry points
=============================================================================*/

#define MEMINI_STRING "CTXMEMINI"

#ifdef USE_MEMINI_CALL_TABLE
#if defined(PLATFORM_PRAGMAS) && defined(MKVERB)
#pragma MKVERB (__FILE__": MEMINI_CALL_TABLE defined")
#endif

#define MEMINI_GLOBAL_INIT PMEMINITABLE g_pMemIniCallTable = NULL
#define MEMINI_EXTERN_INIT extern PMEMINITABLE g_pMemIniCallTable

#define MemIniRunTimeInit(pLibMgr, pVer) \
    ((*pLibMgr->pLMgrUse)(MEMINI_STRING,pVer,(PPLMGRTABLE)&g_pMemIniCallTable))
#define MemIniRunTimeRelease(pLibMgr) \
    ((*pLibMgr->pLMgrRelease)((PLMGRTABLE)g_pMemIniCallTable))

MEMINI_EXTERN_INIT;/* compiling the library itself */

/* compiling someone else who uses the library */

#define miSetProfilePointer (*(g_pMemIniCallTable)->pmiSetProfilePointer)
#define miSetSectionPointer (*(g_pMemIniCallTable)->pmiSetSectionPointer)
#define miGetPrivateProfileString   \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileString)
#ifdef INTERNAL_UTF8
#define miGetPrivateProfileStringInUTF8   \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileStringInUTF8)
#ifdef UNICODE_PROFILE_FUNCTIONS
#define miGetPrivateProfileStringInUTF16   \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileStringInUTF16)
#endif
#endif
#define miGetPrivateProfileInt      \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileInt)
#define miGetPrivateProfileLong     \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileLong)
#define miGetPrivateProfileBool     \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileBool)
#define miReleaseProfile            \
                        (*(g_pMemIniCallTable)->pmiReleaseProfile)
#define miSerialize     \
                        (*(g_pMemIniCallTable)->pmiSerialize)
#define miDeserialize   \
                        (*(g_pMemIniCallTable)->pmiDeserialize)
#define miFreeSerializeData   \
                        (*(g_pMemIniCallTable)->pmiFreeSerializeData)
#define miSetPrivateProfileString   \
                        (*(g_pMemIniCallTable)->pmiSetPrivateProfileString)
#define miAddEvidence   \
                        (*(g_pMemIniCallTable)->pmiAddEvidence)
#define miAuthoriseConnection   \
                        (*(g_pMemIniCallTable)->pmiAuthoriseConnection)
#define miGetPrivateProfileUINT64 \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileUINT64)
#if defined(WIN32) && !defined(WINCE)
#define miGetPrivateProfileStringInUTF8   \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileStringInUTF8)
#define miGetPrivateProfileStringInUTF16   \
                        (*(g_pMemIniCallTable)->pmiGetPrivateProfileStringInUTF16)
#endif
#define miSetPrivateProfileStringInUTF16   \
                        (*(g_pMemIniCallTable)->pmiSetPrivateProfileStringInUTF16)
#define miGetLongCommandLine        \
                        (*(g_pMemIniCallTable)->pmiGetLongCommandLine)
#define miGetPrecedenceAndIcaSetting \
						(*(g_pMemIniCallTable)->pmiGetPrecedenceAndIcaSetting)
#define InitCritSection              \
                        (*(g_pMemIniCallTable)->pmiInitCritSection)
#else /* !USER_MEMINI_CALL_TABLE */

/* compiling the library itself */
int  WFCAPI miSetProfilePointer             (PCHAR);
int  WFCAPI miSetSectionPointer             (PCHAR);
int  WFCAPI miGetPrivateProfileString       (PCHAR, PCHAR, PCHAR, PCHAR, int);
#ifdef INTERNAL_UTF8
int  WFCAPI miGetPrivateProfileStringInUTF8 (PCHAR, PCHAR, PCHAR, PCHAR, int);
#ifdef UNICODE_PROFILE_FUNCTIONS
int  WFCAPI miGetPrivateProfileStringInUTF16(PCHAR, PCHAR, PCHAR, PWCHAR, int);
#endif
#endif
int    WFCAPI miGetPrivateProfileInt          (PCHAR, PCHAR, int);
LONG   WFCAPI miGetPrivateProfileLong         (PCHAR, PCHAR, LONG);
BOOL   WFCAPI miGetPrivateProfileBool         (PCHAR, PCHAR, BOOL);
int    WFCAPI miReleaseProfile();
int    WFCAPI miSerialize                     (PCHAR *, ULONG *);
int    WFCAPI miDeserialize                   (PCHAR, ULONG);
int    WFCAPI miFreeSerializeData             (PCHAR, ULONG);
int    WFCAPI miSetPrivateProfileString       (PCHAR, PCHAR, PCHAR);
int    WFCAPI miAddEvidence                   (LPWSTR, LPSTR);
int    WFCAPI miAuthoriseConnection           (LPSTR);
UINT64 WFCAPI miGetPrivateProfileUINT64       (PCHAR, PCHAR, UINT64);
#if defined(WIN32) && !defined(WINCE)
int    WFCAPI miGetPrivateProfileStringInUTF8 (PCSTR, PCSTR, PCSTR, PCHAR, int);
int    WFCAPI miGetPrivateProfileStringInUTF16(PCSTR, PCSTR, PCSTR, PWCHAR, int);
#endif
int    WFCAPI miSetPrivateProfileStringInUTF16(PCSTR, PCSTR, PCWSTR);
int    WFCAPI miGetLongCommandLine            (PCSTR, PCWSTR, PWCHAR, int);

#define MEMINI_GLOBAL_INIT
#define MEMINI_EXTERN_INIT

#define MemIniRunTimeInit(pLibMgr, pVer)
#define MemIniRunTimeRelease(pLibMgr)
int  WFCAPI miGetPrecedenceAndIcaSetting(PCHAR, PCHAR, INT *, INT *);
BOOL WFCAPI InitCritSection();
#endif  /* !USE_MEMINI_CALL_TABLE */


#ifdef INTERNAL_UTF8
#define miGetPrivateProfileStringInRequiredEncoding      miGetPrivateProfileStringInUTF8
#elif defined(WIN32) && !defined(WINCE)
#define miGetPrivateProfileStringInRequiredEncoding      miGetPrivateProfileStringInUTF8
#else
#define miGetPrivateProfileStringInRequiredEncoding      miGetPrivateProfileString
#endif

/* Temporary wrappers for GetPrivateProfilexxx */
#ifdef unix
#define mitGetPrivateProfileString(a,b,c,d,e,f) miGetPrivateProfileString(a,b,c,d,e)
#define mitGetPrivateProfileInt(a,b,c,d) miGetPrivateProfileString(a,b,c)
#define mitGetPrivateProfileBool(a,b,c,d) miGetPrivateProfileBool(a,b,c)
#else // !unix
DWORD mitGetPrivateProfileString( LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault,
					LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpFileName );
UINT mitGetPrivateProfileInt( LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName );
UINT mitGetPrivateProfileBool( LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName );
#endif  // !unix
/* End wrappers for GetPrivateProfilexxx */


int miSetConfigurationDirectory( PCHAR, PCHAR );
int miLoadICAFile( PCHAR, PCHAR, PCHAR, PCHAR );
int miServerSectionName( LPTSTR, DWORD );
#ifdef UNICODE
#define miServerSectionNameW miServerSectionName
#else /* UNICODE */
int miServerSectionNameW( LPWSTR, DWORD );
#endif /* UNICODE */
struct _ConfigurationManager; /* Declare outside of parenthesis to avoid MS warning C4115 */
int WFCAPI miGetLongCommandLineEx(PCSTR, PCWSTR, PWCHAR, int, struct _ConfigurationManager *);

#ifdef __cplusplus
}
#endif

#endif /* __MIAPI_H__ */
