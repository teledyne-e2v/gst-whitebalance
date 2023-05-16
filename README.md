# gst-whitebalance

# Version 1.0

# About

A Gstreamer plugin to control the white balance of the Optimum color module.

# Dependencies

The following libraries are required for this plugin.
- v4l-utils
- libv4l-dev
- libgstreamer1.0-dev
- libgstreamer-plugins-base1.0-dev
- gcc
- meson 
- ninja

### Debian based system (Jetson): 

```
sudo apt install v4l-utils libv4l-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev meson
```

### Yocto based system (IMX): 

Teledyne provide a bbappend file which provides all packages needed :
https://github.com/teledyne-e2v/Yocto-files

##### Note : You can also compile them on your installed distribution but it will take a long time to compile (Do it only if you miss one or two packages)

# Compilation

## Ubuntu (Jetson)
First you must make sure that your device's clock is correctly setup.
Otherwise the compilation will fail.

In the **gst-whitebalance** folder do:

```
meson build
ninja -C build
sudo ninja -C build install
```


## Yocto (IMX)
First you must make sure that your device's clock is correctly setup.
Otherwise the compilation will fail.

In the **gst-whitebalance** folder do:

```
meson build
ninja -C build install
```


# Installation test

To test if the plugin has been correctly install, do:
```
export GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0/
gst-inspect-1.0 whitebalance
```

If the plugin failed to install the following message will be displayed: "No such element or plugin 'whitebalance'"

# Uninstall
'''
sudo rm /usr/local/lib/gstreamer-1.0/libgstwhitebalance.*
'''
# Usage

By default the plugin is installed in /usr/local/lib/gstreamer-1.0. 
It is then required to tell gstreamer where to find it with the command:
```
export GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0/
```
The plugin can be used in any gstreamer pipeline by adding '''whitebalance''', the name of the plugin.

# Pipeline examples:

With debayer:
```
gst-launch-1.0 v4l2src ! whitebalance ! 'video/x-raw,width=1920,height=1080,format=GRAY8' ! capssetter join=false caps="video/x-bayer,format=rggb"  ! bayer2rgb ! nvvidconv ! 'video/x-raw(memory:NVMM)' ! nv3dsink sync=0
```
With debayer and boosting the blue:
```
gst-launch-1.0 v4l2src ! whitebalance blue="0x03 0x00" ! 'video/x-raw,width=1920,height=1080,format=GRAY8' ! capssetter join=false caps="video/x-bayer,format=rggb"  ! bayer2rgb ! nvvidconv ! 'video/x-raw(memory:NVMM)' ! nv3dsink sync=0
```

With debayer and nvidia plugins (Jetsons only):
```
gst-launch-1.0 v4l2src ! whitebalance ! 'video/x-raw,width=1920,height=1080,format=GRAY8' ! capssetter join=false caps="video/x-bayer,format=rggb"  ! bayer2rgb ! nvvidconv ! 'video/x-raw(memory:NVMM)' ! nv3dsink sync=0
```

By default, the plugin listen for user inputs in the terminal. You can whitebalance and unwhitebalance the video stream by entering '''f''' in the terminal.

# Plugin parameters

- blue:
    - Type: string
    - Default value: "0x01 0xFE"
    - Description: Control the value of blue gain (should be a string of 9 characters as the default value)
- red:
    - Type: string
    - Default value: "0x02 0x36"
    - Description: Control the value of red gain (should be a string of 9 characters as the default value)-  - green:
- green:
    - Type: string
    - Default value: "0x01 0x00"
    - Description: Control the value of green gain (should be a string of 9 characters as the default value)
