/**********************************************************************
* Copyright (C) Citrix Systems, Inc.  All Rights Reserved.
*
* File: Api_HpcCallTable.h
*
* Purpose:
* 
***********************************************************************/
#ifndef __API_HPCCALLTABLE_H__
#define __API_HPCCALLTABLE_H__

#pragma once

#include	<stdio.h>
#include	<stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif
// Provides compatibility to legacy source modules that dont have STDCALL defined

#ifndef STDCALL					
	#define STDCALL	__stdcall
#endif

//===========================================================================================================================
// The following stub is expanded out into a function that calls the function within the call table
//
//  See Call stub definitions section below]
//=============================================================================================================================

#define HPC_CALL_TABLE_STUB( FuncName, ParamType, ...)			\
	__inline static FuncName##_Stub ParamType					\
{																\
	if(g_pHpcCallTable && NULL != g_pHpcCallTable->##FuncName)	\
	{															\
		return( (g_pHpcCallTable->##FuncName)(__VA_ARGS__));	\
	}															\
																\
	return( 0);													\
}

//=============================================================================================================================
// Same as EXTERN_CALL_STUB() above but this version does not return the result from the called function
//=============================================================================================================================
#define HPC_CALL_TABLE_STUB_VOID( FuncName, ParamType, ...)		\
	__inline static FuncName##_Stub ParamType 					\
{																\
	if(g_pHpcCallTable && NULL != g_pHpcCallTable->##FuncName)	\
	{															\
		(g_pHpcCallTable->##FuncName)(__VA_ARGS__);				\
	}															\
}

//=============================================================================================================================
// Macro definition for initializing a call table function address entry
//=============================================================================================================================
#define	HPC_CALL_TABLE_REGISTER_FUNC(a, b)	((a)->b = b)

//=============================================================================================================================
// Macro to set the global call table address
//=============================================================================================================================
#define HPC_CALL_TABLE_REGISTER_TABLE_ADDRESS( pCallTableAddress) g_pHpcCallTable = pCallTableAddress

//=============================================================================================================================
// Macro to make __LINE__ a quoted string. This has to be done in two steps (which is why there are 2 macros) because if
// we try to do it in one step we end up with the word __LINE__ in quotes (ie. "__LINE__") instead of the actual line number
//=============================================================================================================================
#ifndef _HPC_MAKE_QUOTED_STRING
	#define	_HPC_MAKE_QUOTED_STRING(x)	#x
#endif

#ifndef HPC_MAKE_QUOTED_STRING
	#define	HPC_MAKE_QUOTED_STRING(x)	_HPC_MAKE_QUOTED_STRING(x)
#endif

//=============================================================================================================================
// Only one module within a library must specify HPC_CALL_TABLE_REMOTE_ACCESS in a location that is outside of a function definition.
// This creates a global variable which is a pointer to the HPC Call table. 
//
// NOTE: calling the HPC_CALL_TABLE_REGISTER_TABLE_ADDRESS() macro above initializes this variable
//=============================================================================================================================
//#define HPC_CALL_TABLE_REMOTE_ACCESS		HPC_CallTable*  g_pHpcCallTable = NULL; 

//=============================================================================================================================
// Function type. All functions in the Hpc Call Table should be of type HPC_CT_FUNC - ie:  void HPC_CT_FUNC DisplayText( char* pszText)
//=============================================================================================================================
#ifndef HPC_CT_FUNC
	#define HPC_CT_FUNC			__stdcall
#endif

//=============================================================================================================================
//=============================================================================================================================
//=============================================================================================================================
//
// If adding a function to the HPC Call Table update sections 1, 2, and 3 below (DO NOT MODIFY ANYTHING ABOVE)
//
//
//
//                                                    ||||||
//                                                    ||||||
//                                                    ||||||
//                                                    ||||||
//                                                    ||||||
//                                              \\\\\\\\\/////////
//                                               \\\\\\\\////////
//                                                \\\\\\\///////
//                                                 \\\\\\//////
//                                                  \\\\\/////
//                                                   \\\\////
//                                                    \\\///
//                                                     \\//
//                                                      \/
//  
//=============================================================================================================================
//=============================================================================================================================
//=============================================================================================================================

	
//=============================================================================================================================
// 1) Include files
//
// Add any include files required by callers of functions within the call table (these includes should include any prototypes, enums, typedefs, etc)
//	
//=============================================================================================================================
#include <string.h>

//=============================================================================================================================
// 2) HPC Call Table
//
// Add an entry for each function that is available from the call table. 
//
// NOTE: The entry name must match the function name exactly and the arguments specified must match the argument 
//       list for the function exactly.
//
//=============================================================================================================================
typedef struct _HPC_CallTable
{
	// Specify accessable functions below

	// This section of functions are from Hpc_MgrTrace.c which is the HPC Trace subsystem
	int				(HPC_CT_FUNC *Hpc_Trace_AddClassId)(char *pszClassName);
	void			(HPC_CT_FUNC *Hpc_Trace_AddEvent)(unsigned short usMsgNum, unsigned short usWParam, unsigned int uiLParam, void *pTraceParams);
	void			(HPC_CT_FUNC *Hpc_Trace_AddEventHide)(unsigned short usMsgNum, unsigned short usWParam, unsigned int uiParam);
	void			(HPC_CT_FUNC *Hpc_Trace_AddEventWithData)(unsigned short usMsgNum, unsigned short usWParam, unsigned short usLen, unsigned short usDataType, void *ptr, void *pTraceParams);
	void			(HPC_CT_FUNC *Hpc_Trace_AddEventWithDataHide)(unsigned short usMsgNum, unsigned short usWParam, unsigned short usLen, unsigned short usDataType, void *ptr);
	void			(HPC_CT_FUNC *Hpc_Trace_AddEventWithVA)(unsigned short usMsgNum, unsigned short usParam, unsigned short usArgCount, unsigned long dwArgMultibyte, unsigned long dwArgWideString, unsigned long dwArg8Byte, ...);
	unsigned int	(HPC_CT_FUNC *Hpc_Trace_AddFormat)(char *pszFunctionalArea, char *pszModule, char *pszLine, char *pszFunction, char *pszFormat);
	int  			(HPC_CT_FUNC *Hpc_Trace_FlushSegments)(void *pThrdData);
	void			(HPC_CT_FUNC *Hpc_Trace_Init)(void);
	void 			(HPC_CT_FUNC *Hpc_Trace_SignalFlush)(void);
	void 			(HPC_CT_FUNC *Hpc_Trace_Terminate)(unsigned long dwMS);
	unsigned long	(HPC_CT_FUNC *Hpc_Trace_VA_ProcessArgs)(char *pszFmt, unsigned long *pdwArgMultibyte, unsigned long *pdwArgWideString, unsigned long *pdwArg8Byte);
    int             (HPC_CT_FUNC *HPC_Trace_AddFormatedEventToCdfIfEnabled)(char * componentId, char * file, char * line, char * func, unsigned int traceClass, unsigned int level, char * msgFormat, ...);
} HPC_CallTable;
extern HPC_CallTable*	g_pHpcCallTable;

//=============================================================================================================================
// 3) Call stub definitions - FORMAT: function name, (argument types with variable names), arguments
//
//	NOTE: 
//
//		Function name must match the name used in the call table exactly (including case).
//
//		The middle set of values called "argument types with variable names" above must be enclosed in parenthesis () and should 
//			contain the variable type and name. The variable name must match the name (and order) specified in the 3rd set 
//			of values which is referred to as "arguments" above
//
//	Example: if the function pointer in the call table is DisplayText and the prototype is void DisplayText( char* pszText, int nValue)
//			the HPC_CALL_TABLE_STUB macro would be specified as follows:
//
//			void HPC_CALL_TABLE_STUB_VOID( DisplayText, (char* pszText, int nValue), pszText, nValue);
//
//  FOR FUNCTIONS WITH VARIABLE NUMBER OF ARGUMENTS SEE THE SECTION BELOW
//		
//=============================================================================================================================

// This section of functions are from Hpc_MgrTrace.c which is the HPC Trace subsystem

int				HPC_CALL_TABLE_STUB(		Hpc_Trace_AddClassId,			(char* pszClassName), pszClassName);
void			HPC_CALL_TABLE_STUB_VOID(	Hpc_Trace_AddEvent,				(unsigned short usMsgNum, unsigned short usWParam, unsigned int uiLParam, void* pTraceParams), usMsgNum, usWParam, uiLParam, pTraceParams);	
void			HPC_CALL_TABLE_STUB_VOID(	Hpc_Trace_AddEventHide,			(unsigned short usMsgNum, unsigned short usWParam, unsigned int uiParam), usMsgNum, usWParam, uiParam);	
void			HPC_CALL_TABLE_STUB_VOID(	Hpc_Trace_AddEventWithData,		(unsigned short usMsgNum, unsigned short usWParam, unsigned short usLen, unsigned short usDataType, void* ptr, void* pTraceParams), usMsgNum, usWParam, usLen, usDataType, ptr, pTraceParams);
void			HPC_CALL_TABLE_STUB_VOID(	Hpc_Trace_AddEventWithDataHide,	(unsigned short usMsgNum, unsigned short usWParam, unsigned short usLen, unsigned short usDataType, void* ptr), usMsgNum, usWParam, usLen, usDataType, ptr);
unsigned int	HPC_CALL_TABLE_STUB(		Hpc_Trace_AddFormat,			(char* pszFunctionalArea, char* pszModule, char* pszLine, char* pszFunction, char* pszFormat), pszFunctionalArea, pszModule, pszLine, pszFunction, pszFormat);
int				HPC_CALL_TABLE_STUB(		Hpc_Trace_FlushSegments,		(void* pThrdData), pThrdData);
void			HPC_CALL_TABLE_STUB_VOID(	Hpc_Trace_Init,					(void),);
void			HPC_CALL_TABLE_STUB_VOID(	Hpc_Trace_SignalFlush,			(void),);
void			HPC_CALL_TABLE_STUB_VOID(	Hpc_Trace_Terminate,			(unsigned long dwMS), dwMS);
unsigned long	HPC_CALL_TABLE_STUB(		Hpc_Trace_VA_ProcessArgs,		(char* pszFmt, unsigned long* pdwArgMultibyte, unsigned long* pdwArgWideString, unsigned long* pdwArg8Byte), pszFmt, pdwArgMultibyte, pdwArgWideString, pdwArg8Byte);

//=============================================================================================================================
// Stub function that calls Hpc_Trace_AddEventWithVA to generate traces with a sprintf-like call interface.
//=============================================================================================================================

#define HPC_CALL_TABLE_STUB_ADDEVENTWITHVA(szComponentId, pszFile, pszLine, pszFunc, usParam, pszFmt, ...)			\
{																													\
	static	unsigned short		usMsgNum		= 0;                                                                \
	static	unsigned short		usArgCount		= 0;                                                                \
	static	unsigned long		dwArgMultibyte	= 0;                                                                \
	static	unsigned long		dwArgWideString = 0;                                                                \
	static	unsigned long		dwArg8Byte		= 0;                                                                \
                                                                                                                    \
    if(NULL != g_pHpcCallTable->HPC_Trace_AddFormatedEventToCdfIfEnabled)                                           \
    {                                                                                                               \
        if(!g_pHpcCallTable->HPC_Trace_AddFormatedEventToCdfIfEnabled(szComponentId, pszFile, pszLine, pszFunc, CDF_TRACE_CLASS_INFO, CDF_TRACE_LEVEL_URGENT, pszFmt, __VA_ARGS__)) \
        {                                                                                                                              \
            if((0 == usMsgNum) && (NULL != g_pHpcCallTable) && (NULL != g_pHpcCallTable->Hpc_Trace_AddFormat) &&            \
                (NULL != g_pHpcCallTable->Hpc_Trace_VA_ProcessArgs) && (NULL != g_pHpcCallTable->Hpc_Trace_AddEventWithVA)) \
            {                                                                                                               \
                char    szFmt[1024];                                                                                        \
                char*   pcSpecial;                                                                                          \
                                                                                                                            \
                strncpy_s(szFmt, 1023, (pszFmt),_TRUNCATE);                                                   \
                if(NULL != (pcSpecial = strstr( szFmt, "%!")))                                                             \
                {                                                                                                           \
                    *pcSpecial = ' ';                                                                                       \
                }                                                                                                           \
                                                                                                                            \
                usMsgNum = (unsigned short)(g_pHpcCallTable->Hpc_Trace_AddFormat)((szComponentId), (pszFile), (pszLine), (pszFunc), (szFmt));  \
                usArgCount = (unsigned short)(g_pHpcCallTable->Hpc_Trace_VA_ProcessArgs)((pszFmt), &dwArgMultibyte, &dwArgWideString, &dwArg8Byte);  \
            }                                                                                                                \
                                                                                                                             \
            if(0 != usMsgNum)                                                                                                \
            {                                                                                                                \
                (g_pHpcCallTable->Hpc_Trace_AddEventWithVA)(usMsgNum, (usParam), usArgCount, dwArgMultibyte, dwArgWideString, dwArg8Byte, __VA_ARGS__);  \
            }                                                                                                               \
        }                                                                                                                   \
    }                                                                                                                       \
}

//=============================================================================================================================
// Simple sprintf-style macro.  Can be used directly in virtual driver code.
//=============================================================================================================================

#define HPC_TRACEMSG(Fmt, ...)	HPC_CALL_TABLE_STUB_ADDEVENTWITHVA(HPC_MAKE_QUOTED_STRING(ICACLIENT_TRACE_COMPONENT), __FILE__, HPC_MAKE_QUOTED_STRING(__LINE__), __FUNCTION__, 0, Fmt, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif  // __API_HPCCALLTABLE_H__
