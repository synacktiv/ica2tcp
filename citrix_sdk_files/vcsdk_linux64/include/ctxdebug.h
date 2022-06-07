/******************************************************************************
*
*  CTXDEBUG.H
*
*  Header file for Logging and Tracing ASSERT and TRACE macros
*
*  $Id$
*
*  Copyright 1995-2017 Citrix Systems, Inc.  All Rights Reserved.
*
*******************************************************************************/

#ifndef __CTXDEBUG_H__
#define __CTXDEBUG_H__

/*=============================================================================
 ==   ASSERT macro
 ============================================================================*/
#undef  ASSERT

#ifdef DEBUG
#ifdef __GNUC__
#define ASSERT(exp,rc) { \
   if (!(exp)) { \
      LogAssert( #exp, __FILE__, __LINE__, __FUNCTION__, (intptr_t)rc ); \
   } \
}
#else
#if __STDC__ && __STDC_VERSION__ >= 199901L
#define ASSERT(exp,rc) { \
   if (!(exp)) { \
      LogAssert( #exp, __FILE__, __LINE__, __func__, (intptr_t)rc ); \
   } \
}
#else
#define ASSERT(exp,rc) { \
   if (!(exp)) { \
      LogAssert( (PCHAR)#exp, (PCHAR)__FILE__, (int)__LINE__, (intptr_t)(rc) ); \
   } \
}
#endif
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
#undef TRACE_ALWAYS

#if defined(DEBUG)
	#ifndef TRACE_ENABLED
		#define TRACE_ENABLED
	#endif
#endif 

#ifdef TRACE_ENABLED /* For debug version */
        #if defined(NEW_LOGGING) /*  ctxlogd - icasessionmgr, adapter, etc.. */
            #define TRACE_ALWAYS(_arg) {CtxTraceRedir _arg;}
	    #define TRACE(_arg) {CtxTraceRedir _arg;}
            #define TRACEBUF(_arg) {CtxTraceRedir _arg;}
            #define AUTO_TRACE_ENTER(_arg) {CtxTraceRedir _arg;}
            #define DEBUGBREAK() DebugBreak()
        #else /* ncs.log.<pid> - wfica(ncs) */
            #ifndef LOG_WHERE
                    #define LOG_WHERE
            #endif
            #define TRACE_ALWAYS(_arg) {LOG_WHERE LogPrintf _arg;}
            #define TRACE(_arg) {LOG_WHERE LogPrintf _arg;}
            #define AUTO_TRACE_ENTER(_arg) LOG_WHERE int TRACE_RETURN = AutoTrace_Enter_LogPrintf _arg):0
            #undef TRACEBUF
            #define TRACEBUF(_arg) {LOG_WHERE LogBuffer _arg;}
            #define DEBUGBREAK() DebugBreak()
        #endif
#else /* For release version. All components are using new logging (ctxlogd). */
	#ifdef TRACE_FORCEDISABLE
            #define TRACE_ALWAYS(_arg) { }
            #define TRACE(_arg) { }
	#else
            #define TRACE_ALWAYS(_arg) {CtxTraceRedir _arg;}
            #define TRACE(_arg) { }
	#endif
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


#endif  /* __CTXDEBUG_H__ */