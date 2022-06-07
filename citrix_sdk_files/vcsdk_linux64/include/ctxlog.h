/********************************************************************
*
*  ctxlog.h
*
*  Header file for Logging and Tracing
*
*  $Id$
*
*  Copyright 2020 Citrix Systems, Inc. All Rights Reserved.
*
*********************************************************************/

#ifndef __CTXLOG_H__
#define __CTXLOG_H__
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The following macros control which CtxLog functions
 * are enabled during build.
 *
 * CTXLOG_ENABLE_TRACEDEV
 *   Enables/disables CtxTraceDev* functions/macros.
 *
 * CTXLOG_ENABLE_TRACEHEX
 *   Enables/disables CtxTraceHex* functions/macros.
 *
 * CTXLOG_ENABLE_ASSERTIONS
 *   Enables/disables CtxAssert* functions/macros.
 *
 * CTXLOG_ENABLE_LOCATION
 *   Enables/disables inclusion of source file location with log and trace messages.
 **/
    #if DEBUG

        #define CTXLOG_ENABLE_TRACEDEV
        #define CTXLOG_ENABLE_TRACEHEX
        #define CTXLOG_ENABLE_ASSERTIONS
        #define CTXLOG_ENABLE_LOCATION
    #endif




/**
 * Default settings for log rollover.
 **/
    #define LOG_ROLLOVER_SIZE  200
    #define LOG_ROLLOVER_COUNT 2
    #define LOG_SESSION_MAX 200
/**
 * Logging format flags.
 **/
    #define LF_DATE  (1 << 0)
    #define LF_TIME  (1 << 1)
    #define LF_NAME  (1 << 2)
    #define LF_CLASS (1 << 3)
    #define LF_LEVEL (1 << 4)
    #define LF_PID   (1 << 5)
    #define LF_TID   (1 << 6)
    #define LF_UID   (1 << 7)
    #define LF_GID   (1 << 8)
    #define LF_SID   (1 << 9)
    #define LF_FUNC  (1 << 10)
    #define LF_FILE  (1 << 11)
    #define LF_TNAME (1 << 12)

/**
 * Logging classes/components.
 **/
typedef enum
{
    TC_NONE = -1,
    TC_ALL = 0,
    LOG_ASSERT,
    TC_CM,
    TC_CEFBCR,
    TC_CAM,
    TC_CONNCENTER,
    TC_CCM,
    TC_CDM,
    TC_CLIP,
    TC_CPM,
    TC_EUEM,
    TC_FONT,
    TC_FRAME,
    TC_GA,
    TC_IME,
    TC_IPC,
    TC_KEY,
    TC_VDLIC,
    TC_MMVD,
    TC_MOU,
    TC_MTOP,
    TC_LIB,
    TC_PD,
    TC_PN,
    LOG_CLASS,
    TC_SRCC,
    TC_CSM,
    TC_SCARDVD,
    TC_SS,
    TC_SSEXT,
    TC_STF,
    TC_TD,
    TC_TW,
    TC_TWI,
    TC_VD,
    TC_VP,
    TC_UI,
    TC_UIDW3,
    TC_UIDW3E,
    TC_CTXUSB,
    TC_VFM,
    TC_WEBKIT,
    TC_WD,
    TC_NCS,
    TC_WENG,
    TC_WFSHELL,
    TC_WH,
    TC_ZLC,
    TC_APRO,
    TC_SVC,
    TC_DLFCN,
    TC_SBKEYS,
    TC_X11KEYS,
    TC_XCBKEYS,
    TC_X11,
    TC_XCB,
    TC_KC,
    TC_UD, //For UtilDaemon
    TC_LD, //For Launch Darkly
    TC_AN, //For Google Analytics
    LOG_CLASS_COUNT
} LogClass;

/**
 * Logging levels/priorities.
 **/
typedef enum {
    LL_INHERIT = 0,
    LL_VERBOSE = 1,
    LL_INFO = 2,
    LL_WARNING = 3,
    LL_ERROR = 4,
    LL_FATAL = 5,
    LL_AOL = 6,
    LL_NONE = 7,
    LOG_LEVEL_COUNT
} LogLevel;

/**
 * CtxLogInit()
 *
 * Initialise configuration and logging backend.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogInit(
    void
    );

/**
 * CtxLogInitConfigOnly()
 *
 * Initialise configuration but not the logging backend.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogInitConfigOnly(
    void
    );

/**
 * CtxLogShutdown()
 *
 * Shut down configuration and logging backend.
 **/
void
CtxLogShutdown(
    void
    );

/**
 * __CtxLog()
 *
 * Print formatted message with the specified log level.
 *
 * Do not call this function directly, use the
 * CtxLog* macros below instead.
 *
 * @param logLevel Log level of message.
 * @param file     Source file where message was generated.
 * @param func     Function where message was generated.
 * @param line     Source line where message was generated.
 * @param format   Printf-style format string.
 * @param ...      Printf-style argument list.
 **/
void
__CtxLog(
    int logLevel,
    const char *file,
    const char *func,
    int line,
    const char *format,
    ...
    );

#ifdef CTXLOG_ENABLE_LOCATION
    #define CtxLog(ll, format, ...)    __CtxLog(ll, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxLogFatal(format, ...)   __CtxLog(LL_FATAL, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxLogError(format, ...)   __CtxLog(LL_ERROR, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxLogWarning(format, ...) __CtxLog(LL_WARNING, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxLogInfo(format, ...)    __CtxLog(LL_INFO, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxLogVerbose(format, ...) __CtxLog(LL_VERBOSE, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
#else
    #define CtxLog(ll, format, ...)    __CtxLog(ll, "", "", 0, format, ## __VA_ARGS__)
    #define CtxLogFatal(format, ...)   __CtxLog(LL_FATAL, "", "", 0, format, ## __VA_ARGS__)
    #define CtxLogError(format, ...)   __CtxLog(LL_ERROR, "", "", 0, format, ## __VA_ARGS__)
    #define CtxLogWarning(format, ...) __CtxLog(LL_WARNING, "", "", 0, format, ## __VA_ARGS__)
    #define CtxLogInfo(format, ...)    __CtxLog(LL_INFO, "", "", 0, format, ## __VA_ARGS__)
    #define CtxLogVerbose(format, ...) __CtxLog(LL_VERBOSE, "", "", 0, format, ## __VA_ARGS__)
#endif


/**
 * __CtxTraceV()
 *
 * Print formatted trace message with the specified log
 * class and level.
 *
 * Do not call this function directly, use the
 * CtxTraceV* macros below instead.
 *
 * @param logClass Log class to associate message with.
 * @param logLevel Log level of message.
 * @param file     Source file where message was generated.
 * @param func     Function where message was generated.
 * @param line     Source line where message was generated.
 * @param format   Printf-style format string.
 * @param args     Variable Argument List
 **/

void
__CtxTraceV(
    int logClass,
    int logLevel,
    const char *file,
    const char *func,
    int line,
    const char *format,
    va_list args
    );

/**
 * __CtxTrace()
 *
 * Print formatted trace message with the specified log
 * class and level.
 *
 * Do not call this function directly, use the
 * CtxTrace* macros below instead.
 *
 * @param logClass Log class to associate message with.
 * @param logLevel Log level of message.
 * @param file     Source file where message was generated.
 * @param func     Function where message was generated.
 * @param line     Source line where message was generated.
 * @param format   Printf-style format string.
 * @param ...      Printf-style argument list.
 **/
void
__CtxTrace(
    int logClass,
    int logLevel,
    const char *file,
    const char *func,
    int line,
    const char *format,
    ...
    );

#ifdef CTXLOG_ENABLE_LOCATION
    #define CtxTrace(lc, ll, format, ...)    __CtxTrace(lc, ll, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxTraceFatal(lc, format, ...)   __CtxTrace(lc, LL_FATAL, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxTraceError(lc, format, ...)   __CtxTrace(lc, LL_ERROR, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxTraceWarning(lc, format, ...) __CtxTrace(lc, LL_WARNING, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxTraceInfo(lc, format, ...)    __CtxTrace(lc, LL_INFO, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    #define CtxTraceVerbose(lc, format, ...) __CtxTrace(lc, LL_VERBOSE, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
    
    #define CtxTraceV(lc, ll, format, args)    __CtxTraceV(lc, ll, __FILE__, __func__, __LINE__, format, args)
    #define CtxTraceVFatal(lc, format, args)   __CtxTraceV(lc, LL_FATAL, __FILE__, __func__, __LINE__, format, args)
    #define CtxTraceVError(lc, format, args)   __CtxTraceV(lc, LL_ERROR, __FILE__, __func__, __LINE__, format, args)
    #define CtxTraceVAol(lc, format, args)   __CtxTraceV(lc, LL_AOL, __FILE__, __func__, __LINE__, format, args)
    #define CtxTraceVWarning(lc, format, args) __CtxTraceV(lc, LL_WARNING, __FILE__, __func__, __LINE__, format, args)
    #define CtxTraceVInfo(lc, format, args)    __CtxTraceV(lc, LL_INFO, __FILE__, __func__, __LINE__, format, args)
    #define CtxTraceVVerbose(lc, format, args) __CtxTraceV(lc, LL_VERBOSE, __FILE__, __func__, __LINE__, format, args)
#else
    #define CtxTrace(lc, ll, format, ...)    __CtxTrace(lc, ll, "", "", 0, format, ## __VA_ARGS__)
    #define CtxTraceFatal(lc, format, ...)   __CtxTrace(lc, LL_FATAL, "", "", 0, format, ## __VA_ARGS__)
    #define CtxTraceError(lc, format, ...)   __CtxTrace(lc, LL_ERROR, "", "", 0, format, ## __VA_ARGS__)
    #define CtxTraceWarning(lc, format, ...) __CtxTrace(lc, LL_WARNING, "", "", 0, format, ## __VA_ARGS__)
    #define CtxTraceInfo(lc, format, ...)    __CtxTrace(lc, LL_INFO, "", "", 0, format, ## __VA_ARGS__)
    #define CtxTraceVerbose(lc, format, ...) __CtxTrace(lc, LL_VERBOSE, "", "", 0, format, ## __VA_ARGS__)
    
    #define CtxTraceV(lc, ll, format, args)    __CtxTraceV(lc, ll, "", "", 0, format, args)
    #define CtxTraceVFatal(lc, format, args)   __CtxTraceV(lc, LL_FATAL, "", "", 0, format, args)
    #define CtxTraceVError(lc, format, args)   __CtxTraceV(lc, LL_ERROR, "", "", 0, format, args)
    #define CtxTraceVAol(lc, format, args)   __CtxTraceV(lc, LL_AOL, "", "", 0, format, args)
    #define CtxTraceVWarning(lc, format, args) __CtxTraceV(lc, LL_WARNING, "", "", 0, format, args)
    #define CtxTraceVInfo(lc, format, args)    __CtxTraceV(lc, LL_INFO, "", "", 0, format, args)
    #define CtxTraceVVerbose(lc, format, args) __CtxTraceV(lc, LL_VERBOSE, "", "", 0, format, args)
#endif

/**
 * __CtxTraceDev()
 *
 * Print formatted trace message with the specified log
 * class and level.
 *
 * Do not call this function directly, use the
 * CtxTraceDev* macros below instead.
 *
 * This function is intended specifically for
 * DEVELOPMENT ONLY trace messages, such as keyboard
 * logging or user credentials.
 *
 * Messages printed using these functions will NOT be
 * present in release builds.
 *
 * @param logClass Log class to associate message with.
 * @param logLevel Log level of message.
 * @param file     Source file where message was generated.
 * @param func     Function where message was generated.
 * @param line     Source line where message was generated.
 * @param format   Printf-style format string.
 * @param ...      Printf-style argument list.
 **/
    #ifdef CTXLOG_ENABLE_TRACEDEV

void
__CtxTraceDev(
    int logClass,
    int logLevel,
    const char *file,
    const char *func,
    int line,
    const char *format,
    ...
    );

        #define CtxTraceDev(lc, ll, format, ...)    __CtxTraceDev(lc, ll, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
        #define CtxTraceDevFatal(lc, format, ...)   __CtxTraceDev(lc, LL_FATAL, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
        #define CtxTraceDevError(lc, format, ...)   __CtxTraceDev(lc, LL_ERROR, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
        #define CtxTraceDevWarning(lc, format, ...) __CtxTraceDev(lc, LL_WARNING, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
        #define CtxTraceDevInfo(lc, format, ...)    __CtxTraceDev(lc, LL_INFO, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
        #define CtxTraceDevVerbose(lc, format, ...) __CtxTraceDev(lc, LL_VERBOSE, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)

    #else

        #define CtxTraceDev(lc, ll, format, ...)
        #define CtxTraceDevFatal(lc, format, ...)
        #define CtxTraceDevError(lc, format, ...)
        #define CtxTraceDevWarning(lc, format, ...)
        #define CtxTraceDevInfo(lc, format, ...)
        #define CtxTraceDevVerbose(lc, format, ...)
    #endif

/**
 * __CtxTraceHex()
 *
 * Print the contents of a memory block with the specified
 * log class and level.
 *
 * Do not call this function directly, use the
 * CtxTraceHex* macros below instead.
 *
 * Messages printed using these functions will NOT be
 * present in release builds.
 *
 * @param logClass Log class to associate message with.
 * @param logLevel Log level of message.
 * @param file     Source file where message was generated.
 * @param func     Function where message was generated.
 * @param line     Source line where message was generated.
 * @param buf      Memory block to print.
 * @param bufLen   Length of memory block.
 **/
    #ifdef CTXLOG_ENABLE_TRACEHEX

void
__CtxTraceHex(
    int logClass,
    int logLevel,
    const char *file,
    const char *func,
    int line,
    const char *desc,
    const void *buf,
    int bufLen
    );

        #define CtxTraceHex(lc, ll, desc, buf, bufLen)    __CtxTraceHex(lc, ll, __FILE__, __func__, __LINE__, desc, buf, bufLen)
        #define CtxTraceHexFatal(lc, desc, buf, bufLen)   __CtxTraceHex(lc, LL_FATAL, __FILE__, __func__, __LINE__, desc, buf, bufLen)
        #define CtxTraceHexError(lc, desc, buf, bufLen)   __CtxTraceHex(lc, LL_ERROR, __FILE__, __func__, __LINE__, desc, buf, bufLen)
        #define CtxTraceHexWarning(lc, desc, buf, bufLen) __CtxTraceHex(lc, LL_WARNING, __FILE__, __func__, __LINE__, desc, buf, bufLen)
        #define CtxTraceHexInfo(lc, desc, buf, bufLen)    __CtxTraceHex(lc, LL_INFO, __FILE__, __func__, __LINE__, desc, buf, bufLen)
        #define CtxTraceHexVerbose(lc, desc, buf, bufLen) __CtxTraceHex(lc, LL_VERBOSE, __FILE__, __func__, __LINE__, desc, buf, bufLen)

    #else

        #define CtxTraceHex(lc, ll, desc, buf, bufLen)
        #define CtxTraceHexFatal(lc, desc, buf, bufLen)
        #define CtxTraceHexError(lc, desc, buf, bufLen)
        #define CtxTraceHexWarning(lc, desc, buf, bufLen)
        #define CtxTraceHexInfo(lc, desc, buf, bufLen)
        #define CtxTraceHexVerbose(lc, desc, buf, bufLen)
    #endif

/**
 * __CtxAssert()
 *
 * Tests an assert condition, prints log messages and
 * aborts if condition is false.
 *
 * Do not call this function directly, use the
 * CtxAssert macro instead.
 *
 * Assertions will NOT be present in release builds.
 *
 * @param file      Source file where message was generated.
 * @param func      Function where message was generated.
 * @param line      Source line where message was generated.
 * @param condition Condition to assert.
 * @param message   Description of assert condition.
 **/
    #ifdef CTXLOG_ENABLE_ASSERTIONS

void
__CtxAssert(
    const char *file,
    const char *func,
    int line,
    int condition,
    const char *format,
    ...
    );

        #define CtxAssert(condition, format, ...) __CtxAssert(__FILE__, __func__, __LINE__, !!(condition), format, ## __VA_ARGS__)

    #else

        #define CtxAssert(...)
    #endif

/**
 * CtxLogEnableFlags()
 *
 * Enable the specified format flags.
 *
 * @param logFlags Flags to enable.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogEnableFlags(
    unsigned int logFlags
    );

/**
 * CtxLogDisableFlags()
 *
 * Disable the specified format flags.
 *
 * @param logFlags Flags to disable.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogDisableFlags(
    unsigned int logFlags
    );

/**
 * CtxLogGetCurrentFlags()
 *
 * Get current format flags.
 *
 * @param logFlags Location to receive flags.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogGetCurrentFlags(
    unsigned int *logFlags
    );

/**
 * CtxLogGetDefaultFlags()
 *
 * Get default format flags.
 *
 * @param logFlags Location to receive flags.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogGetDefaultFlags(
    unsigned int *logFlags
    );

/**
 * CtxLogSetCurrentLevel()
 *
 * Set the current log level for the specified class.
 *
 * @param logClass Logging class to modify.
 * @param logLevel Logging level to set.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogSetCurrentLevel(
    int logClass,
    int logLevel
    );

/**
 * CtxLogGetCurrentLevel()
 *
 * Get the current log level for the specified class.
 *
 * @param logClass Logging class to query.
 *
 * @return Current log level of specified class, or -1
 *         if an error occurs.
 **/
int
CtxLogGetCurrentLevel(
    int logClass
    );

/**
 * CtxLogGetWorkingLevel()
 *
 * Get the working log level for the specified class
 * by querying its parent classes if necessary.
 *
 * @param logClass Logging class to query.
 *
 * @return Working log level of specified class, or -1
 *         if an error occurs.
 **/
int
CtxLogGetWorkingLevel(
    int logClass
    );

/**
 * CtxLogGetDefaultLevel()
 *
 * Get the default log level for the specified class.
 *
 * @param logClass Logging class to query.
 *
 * @return Default log level of specified class, or -1
 *         if an error occurs.
 **/
int
CtxLogGetDefaultLevel(
    int logClass
    );

/**
 * CtxLogGetParent()
 *
 * Returns the parent class of the specified log class.
 *
 * @param logClass Logging class to query.
 *
 * @return Parent of specified log class, or -1
 *         if class has no parent or an error occurs.
 **/
int
CtxLogGetParent(
    int logClass
    );

/**
 * CtxLogGetClassName()
 *
 * Returns the name of the specified log class.
 *
 * @param logClass Logging class to query.
 *
 * @return Log class name, or NULL on error.
 **/
const char *
CtxLogGetClassName(
    int logClass
    );

/**
 * CtxLogGetClassDisplayName()
 *
 * Returns the display name of the specified log class.
 *
 * @param logClass Logging class to query.
 *
 * @return Log class display name, or NULL on error.
 **/
const char *
CtxLogGetClassDisplayName(
    int logClass
    );

/**
 * CtxLogGetLevelName()
 *
 * Returns the name of the specified log class.
 *
 * @param logClass Logging class to query.
 *
 * @return Log class name, or NULL on error.
 **/
const char *
CtxLogGetLevelName(
    int logClass
    );

/**
 * CtxLogGetLevelDisplayName()
 *
 * Returns the display name of the specified log class.
 *
 * @param logClass Logging class to query.
 *
 * @return Log class display name, or NULL on error.
 **/
const char *
CtxLogGetLevelDisplayName(
    int logClass
    );

/**
 * CtxLogSetOutputPath()
 *
 * Set the current log output path.
 *
 * @param path New output path.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogSetOutputPath(
    const char *path
    );

/**
 * CtxLogGetOutputPath()
 *
 * Returns the current log output path.
 *
 * @return Current output path, or NULL on error.
 **/
const char *
CtxLogGetOutputPath(
    void
    );

/**
 * CtxLogSetRolloverSize()
 *
 * Set the file size at which log rollover will occur.
 *
 * @param size File size in MiB.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogSetRolloverSize(
    int size
    );

/**
 * CtxLogGetRolloverSize()
 *
 * Returns the file size at which log rollover will occur.
 *
 * @return Current rollover size in MiB, or -1 on error.
 **/
int
CtxLogGetRolloverSize(
    void
    );

/**
 * CtxLogSetRolloverCount()
 *
 * Set the maximum number of archived log files to keep when rollover occurs.
 * Specifying zero will disable rollover.
 *
 * @param count Desired number of files.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogSetRolloverCount(
    int count
    );

/**
 * CtxLogGetRolloverCount()
 *
 * Returns the current maximum number of archived log files.
 *
 * @return Current maximum number of files, 0 if rollover is disabled, or -1 on error.
 **/
int
CtxLogGetRolloverCount(
    void
    );

/**
 * CtxLogSetSessionMax()
 *
 * Set the most sessions logd can create.
 *
 * @param sessionMax Max session number.
 *
 * @return True on success, false otherwise.
 **/
int
CtxLogSetSessionMax(
    int sessionMax
    );

/**
 * CtxLogGetSessionMax()
 *
 * Returns the most sessions logd can create.
 *
 * @return Current max session number, or -1 on error.
 **/
int
CtxLogGetSessionMax(
    void
    );

/**
 * LogSafeStr()
 **/
static inline const char *
LogSafeStr(
    const char *str
    )
{
    return str ? str : "(NULL)";
}

#ifdef __cplusplus
}
#endif

#endif /* __CTXLOG_H__ */
