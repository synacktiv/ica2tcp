# Client DLL for Linux

## Build

The build process is based on CMake.

### Installing the Citrix SDK
It is first necessary to install the Citrix Virtual Channel SDK for Linux (VCSDK). In order to do so, three solutions : 
- Use the Citrix files provided in this repository under `ica2tcp/citrix_sdk_files`
- Install custom-citrix-sdk-dev
- Dowload the SDK from the Citrix website and install it manually


#### Use provided files
Easiest solution, nothing to do.

#### Custom-citrix-sdk-dev
Run 
```
apt install custom-citrix-sdk-dev
```

This will install several header files in `/usr/include/citrix` and the `vdapi.a` library in `/usr/lib/x86_64-linux-gnu/citrix/`.

In `CMakeLists.txt`: 
- Uncomment the `#add_compile_definitions(SDK)` line
- Update the line `target_link_libraries(ica2tcp_client_dll_linux ${CMAKE_SOURCE_DIR}/../citrix_sdk_files/vcsdk_linux64/lib/vdapi.a)` with the right path to `vdapi.a`


#### Manual install
Dowload the SDK on the Citrix website : 
`https://www.citrix.com/downloads/workspace-app/virtual-channel-sdks/virtual-channel-sdk.html` (this requires creating an account)
 
Create a `citrix` folder in `/usr/include` or anywhere else in your include path. Decompress the downloaded archive and copy all the files from the following folders into `citrix` : 
- `VCSDK/base/inc/`
- `VCSDK/base/inc/unix/`
- `VCSDK/base/shared/inc/citrix/`
- `VCSDK/base/<PLATEFORM>/inc/` where `<PLATEFORM>` is one of linux, linux64, linuxarm, linuxarmhf

Copy `VCSDK/binaries/lib/<PLATEFORM>/retail/vdapi.a` into `/usr/lib/x86_64-linux-gnu/citrix/` (you can copy this file somewhere else but you will need to fix the `find_library()` in `CMakeLists.txt`).

### Installing CMake

Run 
```
apt install cmake
```

### Building the DLL

Inside the `ica2tcp/client_dll_linux` folder, run : 
```
cmake -DCMAKE_BUILD_TYPE:STRING=<Release or Debug> -S . -B build
```

Then, run 
```
cmake --build build
```
If everything went well, the DLLs `DRIVER.DLL` and/or `DRIVER_DEBUG.DLL` are created in `ica2tcp/client_dll_linux/build/`

## Install 
On the Citrix client, copy the DLL to `<CITRIX_INSTALL_FOLDER>/ICAClient/<NAME>.DLL`
Modify the `<CITRIX_INSTALL_FOLDER>/ICAClient/config/module.ini` as follows.

Create a section `[<SECTION_NAME>]` and add : 

```
[<SECTION_NAME>]
DriverName = <NAME>.DLL
LogPath = /path/to/a/logging/folder/
```

In `[ICA 3.0]` section, add `<SECTION_NAME>` to the `VirtualDriver` list.
Still in `[ICA 3.0]`, add: 
```
<SECTION_NAME> = On
```