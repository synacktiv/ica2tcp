//
// Created by hugoc on 04/06/2021.
//

#ifndef VDSOCKS_DEBUG_H
#define VDSOCKS_DEBUG_H


#include <stdio.h>
#include <string.h>

#define LOG_PATH "socks2.txt"
#define POLL_LOG_PATH "socks_poll2.txt"
#define UNSENT_LOG_PATH "socks_unsent2.txt"
#define SQUEUE_LOG_PATH "socks_squeue2.txt"
#define ERROR_LOG_PATH "socks2_errors.txt"

#ifdef DEBUG_ON

#define DEBUG_VAR(file,fmt, ...) do {fprintf(file, "%s:%d:" fmt, __func__, __LINE__, __VA_ARGS__);fflush(file);} while(0);
#define DEBUG(file, message) do {fprintf(file, "%s:%d:" message, __func__, __LINE__);fflush(file);} while(0);
#define DEBUG_INFO(file) do {debug_info(file);} while(0);
#define DEBUG_BUF(buf, len) do {debugBuf(buf, len);} while(0);
#else
#define DEBUG_VAR(file, fmt, ...) do {} while(0);
#define DEBUG(file, message) do {} while(0);
#define DEBUG_INFO(file) do {} while(0);
#define DEBUG_BUF(buf, len) do {} while(0);

#endif

#ifdef WARN_ON
#define DEBUG_WARN_VAR(file,fmt, ...) do {fprintf(file,"%s:%d:" fmt, __func__, __LINE__, __VA_ARGS__);fflush(file);} while(0);
#define DEBUG_WARN(file,message) do {fprintf(file, "%s:%d:" message, __func__, __LINE__);fflush(file);} while(0);
#define DEBUG_WARN_INFO(file) do {debug_info(file);} while(0);
#define DEBUG_WARN_BUF(buf, len) do {debugBuf(buf, len);} while(0);
#else
#define DEBUG_WARN_VAR(file,fmt, ...) do {} while(0);
#define DEBUG_WARN(file,message) do {} while(0);
#define DEBUG_WARN_INFO(file) do {} while(0);
#define DEBUG_WARN_BUF(buf, len) do {} while(0);
#endif 


#ifdef DEBUG_ALWAYS_ON
#define DEBUG_ALWAYS_VAR(file,fmt, ...) do {fprintf(file,"%s:%d:" fmt, __func__, __LINE__, __VA_ARGS__);fflush(file);} while(0);
#define DEBUG_ALWAYS(file,message) do {fprintf(file, "%s:%d:" message, __func__, __LINE__);fflush(file);} while(0);
#define DEBUG_ALWAYS_INFO(file) do {debug_info(file);} while(0);
#define DEBUG_ALWAYS_BUF(buf, len) do {debugBuf(buf, len);} while(0);
#else
#define DEBUG_ALWAYS_VAR(file,fmt, ...) do {} while(0);
#define DEBUG_ALWAYS(file,message) do {} while(0);
#define DEBUG_ALWAYS_INFO(file) do {} while(0);
#define DEBUG_ALWAYS_BUF(buf, len) do {} while(0);

#endif


#endif //VDSOCKS_DEBUG_H

void DebugInit(char basePath[180]);