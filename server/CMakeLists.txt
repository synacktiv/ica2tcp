cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 99)
set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_C_COMPILER "/usr/bin/i686-w64-mingw32-gcc")

project(ica2tcp_server C)

add_compile_definitions(WIN32)
add_compile_definitions(_WIN32)
add_compile_definitions(NDEBUG)
add_compile_definitions(_LIB)
add_compile_definitions(_WIN32_WINNT=0x0600)
add_compile_options(-Wall -v)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_ON)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(PROD)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")


# Additional directories to look for header files 
# ----------------------------------------------- 

# common header file for ica2tcp server and client dll
include_directories(../common/)

# vcsdk shared headers (needed both for client DLL and server app)
include_directories(../citrix_sdk_files/vcsdk_win32/include/shared/)

# wfapi headers (wfapi.h and CtxSmcApi.h)
include_directories(../citrix_sdk_files/wfapi_sdk_win32/include/)

add_executable(ica2tcp_server ctxsocks_v2.c  connection_manager_win.c debug_win.c  socket_queue_win.c)
set_target_properties(ica2tcp_server PROPERTIES OUTPUT_NAME_RELEASE "server")
set_target_properties(ica2tcp_server PROPERTIES OUTPUT_NAME_DEBUG "server_debug")

# Link the WFAPI SDK static library 
target_link_libraries(ica2tcp_server ${CMAKE_SOURCE_DIR}/../citrix_sdk_files/wfapi_sdk_win32/lib/wfapi.lib)

# Link WinSock library
find_library(WS2_32_LIB libws2_32.a HINTS /usr/i686-w64-mingw32/lib)
target_link_libraries(ica2tcp_server ${WS2_32_LIB})