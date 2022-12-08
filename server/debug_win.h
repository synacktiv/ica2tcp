//
// Created by hugoc on 04/06/2021.
//

#ifndef DEBUG_WIN_H
#define DEBUG_WIN_H
#include <stdio.h>

#ifdef DEBUG_ON


#define DEBUG_VAR(fmt, ...) do {printf("%s:%d:" fmt, __func__, __LINE__, __VA_ARGS__);} while(0);
#define DEBUG(message) do {printf("%s:%d:" message, __func__, __LINE__);} while(0);
#define DEBUG_INFO() do {debug_info();} while(0);
#define DEBUG_BUF(buf, len) do {debugBuf(buf, len);} while(0);

#else

#define DEBUG_VAR(fmt, ...) do {} while(0);
#define DEBUG(message) do {} while(0);
#define DEBUG_INFO() do {} while(0);
#define DEBUG_BUF(buf, len) do {} while(0);

#endif

#ifdef WARN_ON


#define DEBUG_WARN_VAR(fmt, ...) do {printf("%s:%d:" fmt, __func__, __LINE__, __VA_ARGS__);} while(0);
#define DEBUG_WARN(message) do {printf("%s:%d:" message, __func__, __LINE__);} while(0);
#define DEBUG_WARN_INFO() do {debug_info();} while(0);
#define DEBUG_WARN_BUF(buf, len) do {debugBuf(buf, len);} while(0);

#else

#define DEBUG_WARN_VAR(fmt, ...) do {} while(0);
#define DEBUG_WARN(message) do {} while(0);
#define DEBUG_WARN_INFO() do {} while(0);
#define DEBUG_WARN_BUF(buf, len) do {} while(0);

#endif

#ifdef DEBUG_ALWAYS_ON


#define DEBUG_ALWAYS_VAR(fmt, ...) do {printf("%s:%d:" fmt, __func__, __LINE__, __VA_ARGS__);} while(0);
#define DEBUG_ALWAYS(message) do {printf("%s:%d:" message, __func__, __LINE__);} while(0);
#define DEBUG_ALWAYS_INFO() do {debug_info();} while(0);
#define DEBUG_ALWAYS_BUF(buf, len) do {debugBuf(buf, len);} while(0);

#else

#define DEBUG_ALWAYS_VAR(fmt, ...) do {} while(0);
#define DEBUG_ALWAYS(message) do {} while(0);
#define DEBUG_ALWAYS_INFO() do {} while(0);
#define DEBUG_ALWAYS_BUF(buf, len) do {} while(0);

#endif



#endif //DEBUG_WIN_H

void DebugInit();