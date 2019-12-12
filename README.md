# BSides Cape Town 2019 Badge

Welcome to the repo for the BSides Cape Town 2019 Badge :)

# About
This is the firmware repo for this badge, this is all the software and tooling used in the development of the badges (outside of the Arduino IDE / ESP32 toolchain and TFT_eSPI library)

# Why should I expect to be able to do
You should be able to do anything you want with this code and the badge.

Including and not limited to :
- You want the latest version of the firmware
- You want to get to know more about the badge
- You want to hack the badge and make it do something else
- You want to fix a bug in the code (There's always bugs)

# Getting started
## Pre-requisites
If you're looking to get started with the development you will need a couple pre-requisites.

- Arduino IDE (1.8.10 used)
- ESP32 toolchain (1.0.4)
- TFT_eSPI library (1.4.20 via Library manager)
- SDL for Windows / Linux debugging.

A lof of the team use Visual Studio Code, but the Arduino IDE or your favourite editor should be fine too.

Everything else is built into the source code here.

- JSON library
- JPEG library
- QRCodeGenerator library

## Code
Once you're setup and ready to go, pull this repo locally to your machine.

TFT_eSPI need some special setup, you can do this by copying the files from the extra/tft_espi folder to you library folder, this is under Documents/Arudino on Windows, on Linux (/usr/share/arduino/libraries/usr/share/arduino/libraries)

What these files do is setup the badges pins and some options for communication from the ESP32 to the Screen.

The settings for the ESP32 are 
- Board : ESP32 Dev Module
- Port Speed : 921600
- Speed : 240mhz WIFI/BT
- Flash Frequency : 80Mhz
- Flash Mode   : QIO
- Flash Size : 4Mb
- Partition Scheme : Huge App (3mb Firmware / 1Mb SPIFFS)
- Core Debug Level : None
- PSRAM : Disabled
- Port : Once your badge is plugged in and on, choose the correct port.

## Compile
### For Local :
- Make sure SDL is installed.
- From build directory in repo (create if doesn't exist)
- Run "cmake .."
- Then "run make .."

### For Badge : 
Open up the BSidesCapeTown2019-Firmware.ino file and compile

# Code Primer
## Important code
The code is mostly driven around being platform agnostic via the game.hpp file. This in turn loops in the fonts and core libraries needed for the badge as well as interfacing the platform game_sdl or esp32 depending on the ifdefs at compile time.

In game.cpp you will see the code that drives the engine frame to frame, it handles initialisation for the different platforms as well as the core game loop.

This in turn calls mygame.cpp which is the start of what I would consider user code, rather than framework code.

The GameBuff object is the runtime object for manipulating the screen as well and getting input from the badge.

## Important objects
- gameBuff->WIDTH and HEIGHT - variables containing the width and height of the screen
- gameBuff->consoleBuffer - is a single dimensional array which represents your screen, it is maintained between frames and you manually need to clear it if you want to. To access and set X and Y co-ordinates is [X + Y * WIDTH]
- gameBuff-playerKeys - contains the input from the badged, this is read per frame. The debouncedInput will tell allow you to get cleaner input every 300ms.

## Important procedures
Take a look through game.hpp, but some of the big getting started methods are
- displayClear - Clear the display
- drawObject - Draw a unsigned char array (256 colours, per byte, stored in RGB332 format)
- drawBlock - Draw a block on the screen
- drawString - Draw a string on the screen

They either take X and y or Dimension object, the Dimension object is X,Y,Width and height.

