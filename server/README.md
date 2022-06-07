# Server executable (Windows only)

## Build

The build process is based on CMake and MinGW for cross-compilation (on Linux, for Win32)

### Install MinGW
Packaged for Debian. Run: 
```
apt install mingw-w64
```

### Install CMake
Run 
```
apt install cmake
```

### Install Citrix SDKs
To build the server side executable, some files of the Virtual Channel SDK are needed, as well as the WFAPI SDK. The WFAPI SDK is provided by Citrix as a `.msi`. So the easiest way is to use all the Citrix files provided in this repository (you do not need to do anything). However, it is possible to manually download and install the SDKs (VCSDK for Windows and WFAPI SDK). In this case, you need to update `CMakeLists.txt` with the right paths to the different files required:
- Link the `wfapi.lib` static library
- Add the path to `wfapi.h` and `CtxSmcApi.h` header files
- Add the path to `ica.h`, `ica-c2h.h`, `icaid.h`, `vrtlclss.h` header files

### Build the server

Inside the `ica2tcp/server` folder, run : 
```
cmake -DCMAKE_BUILD_TYPE:STRING=<Release or Debug> -S . -B build
```

Then, run 
```
cmake --build build
```
If everything went well, the executables `server.exe` and/or `server_debug.exe` are created in `ica2tcp/server/build/`

## Usage
Copy the executable on the Citrix server and execute it from within an ICA session (otherwise the virtual channel cannot be opened)