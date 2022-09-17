# Panasonic Camera Reverse (Use your Panasonic Camera on Linux)
Applications Created by reverse enginnering the Panasonics Cameras

This Repository Contains some Applications created by Reverse enginnering Panasonic Cameras

## The Utilities
For a Live Feed of your Camera use `PanLivePlayerControl`.
For an v4l2 feed of your Camera use Panasonic_to_v4l2`.

## Tested Platforms
Currently only Linux is supported.
Macos was tried but SDL2(NSWindow) doesn't allow to create a window inside of an Thread it might be fixed in a later version.

## PanLivePlayerControl
This is a Application Written in C/C++ which gives you a Live feed of your Camera using sdl2.

### Dependencies
PanLivePlayerControl uses : g++ SDL2 SDL2_image libcurl and libturbojpeg
Ubuntu/Debain : `sudo apt install libsdl2-dev libsdl2-image-dev libcurl-dev libjpeg-turbo libao-dev`

### Build
Just run the `build.sh` script in the directory to build it.

### Usage
Run `./PanLivePlayerControl <The IP of your Camera>`

## Panasonic_to_v4l2
This captures the Live feed of your Camera to an v4l2 device.
### Build

Just run the `build.sh` script in the directory to build it.

### Dependencies
PanasonicToV4l2 uses : [libv4l2cpp](https://github.com/mpromonet/libv4l2cpp) libcurl v4l2loopback(Fake v4l2 device)  and g++
Ubuntu/Debain : `sudo apt install libcurl-dev v4l2loopback-dkms`

### Usage
1. Create a Fake v4l2 device (`sudo modprobe v4l2loopback video_nr=<The Number of the v4l2 device>`)
2. `./PanasonicToV4l2 <The IP of the Cmaera> /dev/video<The same number you selected before>`

## Tested Cameras
Currently only the HC-V380 was tested but if this works with other cameras please create an issue.

## Todos
The Code isn't optimized yet and still has tons of flaws see it as a proof of concept.
If you fixed an issue or cleared up the code please create an PR.
