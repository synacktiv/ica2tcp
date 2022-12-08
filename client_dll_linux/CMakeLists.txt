cmake_minimum_required(VERSION 3.13)
project(ica2tcp_client_dll_linux C)

set(CMAKE_C_STANDARD 99)

add_compile_definitions(VCSDK)
add_compile_definitions(unix)
add_compile_definitions(Linux)
add_compile_definitions(INTERNAL_UTF8)
add_compile_definitions(WRAP_LOAD=ica2tcp_WrapLoad)

#add_compile_definitions(SDK)



add_compile_options(-Wall -v)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_NAME)
    add_compile_definitions(DEBUG_ON)
    add_compile_definitions(DEBUG_ALWAYS_ON)
    add_compile_definitions(WARN_ON)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-fstack-protector-all )
    add_compile_definitions(DEBUG_ALWAYS_ON)
    add_compile_definitions(WARN_ON)
    add_compile_definitions(PROD)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")


# additionnal directories to look for header files
include_directories(../common/)
include_directories(../citrix_sdk_files/vcsdk_linux64/include/)
include_directories(../citrix_sdk_files/vcsdk_linux64/include/shared/)

add_library(ica2tcp_client_dll_linux SHARED vdsocks_v2.c  connection_manager.c socket_queue.c debug.c ica_queue.c )

# rename the output files
set_target_properties(ica2tcp_client_dll_linux PROPERTIES PREFIX "")
set_target_properties(ica2tcp_client_dll_linux PROPERTIES SUFFIX ".DLL")
set_target_properties(ica2tcp_client_dll_linux PROPERTIES OUTPUT_NAME_RELEASE "DRIVER")
set_target_properties(ica2tcp_client_dll_linux PROPERTIES OUTPUT_NAME_DEBUG "DRIVER_DEBUG")

# link the Citrix VDAPI static library vdapi.a
target_link_libraries(ica2tcp_client_dll_linux ${CMAKE_SOURCE_DIR}/../citrix_sdk_files/vcsdk_linux64/lib/vdapi.a)