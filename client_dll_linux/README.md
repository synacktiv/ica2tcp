# Client DLL for Linux

## Build

The build process is based on CMake.

### Citrix SDK
It is first necessary to install the Citrix Virtual Channel SDK for Linux (VCSDK). In order to do so, two solutions: 
- Use the Citrix files provided in this repository ([here](../citrix_sdk_files/))
- Dowload the SDK from the Citrix website and install it manually

It is easier to use the supplied files as the `CMakeLists.txt` file is already configured for this. However, feel free to manually install the SDK wherever you want on your machine and to adapt the file and the build process. For more information about the libraries and headers required, please refer to [CMakeLists.txt](./CMakeLists.txt).
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
Once built, you have to install the client shared library.

First, copy the build output file `DRIVER.DLL` to `<CITRIX_INSTALL_FOLER>/ICAClient/<DRIVER_NAME>.DLL`

Then, modify the `<CITRIX_INSTALL_FOLER>/ICAClient/config/module.ini` configuration file as follows: 
- Add `<VIRTUAL_DRIVER_NAME>` in the `VirtualDriver` list in `[ICA 3.0]` section (replace `<VIRTUAL_DRIVER_NAME>` with anything you want).
- Still in `[ICA 3.0]` section, add the line `<VIRTUAL_DRIVER_NAME>=On`
- Add a `[<VIRTUAL_DRIVER_NAME>]` section containing:
```
DriverName = <DRIVER_NAME>.DLL
LogPath = /path/to/logs/folder/
Port = <PORT_NUMBER_FOR_SOCKS>  //default: 33556
Address = <LISTENING_IP_ADDRESS>  //default: 127.0.0.1, use 0.0.0.0 to open the socks for non local clients
```

That's all!