/******************************************************************************
*
*  CTXDEBUG.H
*
*  Header file for Logging and Tracing ASSERT and TRACE macros
*
*  $Id: //RfWin/Release-Kiwi/CitrixReceiver/src/inc/ctxdebug.h#1 $
*
*  Copyright 1998-2015 Citrix Systems, Inc.  
*
*******************************************************************************/

#ifndef __CTXDEBUG_H__
#define __CTXDEBUG_H__

#pragma once

/*=============================================================================
 ==   ASSERT macro
 ============================================================================*/
#undef  ASSERT

#if defined(DEBUG) || defined(_DEBUG)
#ifdef __GNUC__
#define ASSERT(exp,rc) { \
   if (!(exp)) { \
      LogAssert( #exp, __FILE__, __LINE__, __FUNCTION__, (int)rc ); \
   } \
}
#else
#define ASSERT(exp,rc) { \
   if (!(exp)) { \
      LogAssert( (PCHAR)#exp, (PCHAR)__FILE__, (int)__LINE__, (int)(rc) ); \
   } \
}
#endif

#define Int3(a) LogPrintf(0xffffffff, 0xffffffff, a);

#else
#define ASSERT(exp,rc) { }
#define Int3(a)
#endif


/*=============================================================================
 ==   TRACE macros
 ============================================================================*/
#undef TRACE

#if defined(DEBUG) || defined(_DEBUG)
#ifndef TRACE_ENABLED
#define TRACE_ENABLED
#endif
#endif

#ifdef TRACE_ENABLED

#ifndef LOG_WHERE
#define LOG_WHERE
#endif

#define TRACE(_arg) {LOG_WHERE LogPrintf _arg;}
#define AUTO_TRACE_ENTER(_arg) LOG_WHERE int TRACE_RETURN = AutoTrace_Enter_LogPrintf _arg):0
#undef TRACEBUF
#define TRACEBUF(_arg) {LOG_WHERE LogBuffer _arg;}
#define DEBUGBREAK() DebugBreak()
#else
#define TRACE(_arg) { }
#define TRACEBUF(_arg) { }
#define AUTO_TRACE_ENTER(_arg) { }
#define DEBUGBREAK() { }
#endif

/* Specifically defined to nothing in order to
 * be able to define away unwanted TRACE statements
 * by just changing TRACE to DTRACE */
#define DTRACE(_arg) { }
#define DTRACEBUF(_arg) { }
#define DASSERT(exp,rc) { }

/*=============================================================================
 ==   DeclareFName macro
 ==   NB. Used without a semicolon
 ============================================================================*/

/*  Debug Macro */
#ifdef TRACE_ENABLED
#define DeclareFName(func) const char fName[] = func;
#else
#define DeclareFName(func)
#endif

#endif  /* __CTXDEBUG_H__ */
