# About Ica2Tcp

Ica2Tcp is a C tool allowing to proxy any TCP connection inside an Citrix ICA connection. It is to Citrix what `ssh -D` is to SSH.

# Disclaimer

This tool was developped as an internship project and is still under developpment. It is published "as is", so some parts of the code are still a bit dirty. A cleaner version is in progress and will be released soon.
However, any remark concerning this version is welcome !

# Build the tool

Both the server side standalone binary and the client side shared library must be built.
Please refer to the corresponding README for more details:
- [Server binary](./server/README.md)
- [Client shared library](./client_dll_linux/README.md) 

For convenience, all the required Citrix headers and libraries have been included in this repository ([here](/citrix_sdk_files/)). They all originate from the Virtual Channel SDK and WFAPI SDK that are publicly available: 
- https://www.citrix.com/downloads/workspace-app/virtual-channel-sdks/virtual-channel-sdk.html
- https://developer.cloud.com/archived-sdks/docs/archived-sdks#winframe-api-sdk 
  
Feel free to manually download and install the SDK's, however you might have to change the build process a bit.

# Install 

Once everything is built, you have to install the client shared library.

First, copy the build output file `DRIVER.DLL` to `<CITRIX_INSTALL_FOLER>/ICAClient/<DRIVER_NAME>.DLL`

Then, modify the `<CITRIX_INSTALL_FOLER>/ICAClient/config/module.ini` configuration file as follows: 
- Add `<VIRTUAL_DRIVER_NAME>` in the `VirtualDriver` list in `[ICA 3.0]` section (replace `<VIRTUAL_DRIVER_NAME>` with anything you want).
- Still in `[ICA 3.0]` section, add the line `<VIRTUAL_DRIVER_NAME>=On`
- Add a `[<VIRTUAL_DRIVER_NAME>]` section containing:
```
DriverName = <DRIVER_NAME>.DLL
LogPath = /path/to/logs/folder/
```

That's all!

# Usage

Once the client side driver is correctly installed, you just need to copy the server binary on the server and execute it from an active ICA session.
It opens a SOCKS service on the client side on port 33556.