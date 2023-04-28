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
- meson (>= 0.49)
- ninja
- gstreamer-1.0


### Debian based system (Jetson): 

```
sudo apt install v4l-utils libv4l-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
```
Meson > 0.49 is required, you can download the good version on the official debian repositories :
https://packages.debian.org/buster/all/meson/download.

Once you have download your package, you can install it with the command : 
```
sudo apt install ./meson_0.49.2-1_all.deb
```

This should install the ninja package, if not use the command : 
```
sudo apt install ninja
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
With fake image pipeline:
```
gst-launch-1.0 videotestsrc ! whitebalance ! videoconvert ! ximagesink
```

With simple video stream:
```
gst-launch-1.0 v4l2src ! whitebalance ! queue ! videoconvert ! queue ! xvimagesink sync=false
```

By default, the plugin listen for user inputs in the terminal. You can whitebalance and unwhitebalance the video stream by entering '''f''' in the terminal.

# Plugin parameters

- freeze:
    - Type: boolean
    - Default value: false
    - Description: Freeze the stream without blocking the pipeline

- listen:
    - Type: boolean
    - Default value: true
    - Description: Listen for user inputs in the terminal
