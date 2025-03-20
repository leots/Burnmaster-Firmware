# Funnyplaying Burnmaster Firmware

Based on Sanni's [cartreader](https://github.com/sanni/cartreader) firmware

## Fork info

Changes in this fork:
- Fixed incrementing of folders for ROMs & saves by reading the SD card instead of depending on the EEPROM which didn't work on my Burnmaster. You can now dump saves & ROMs multiple times directly from the unit (useful for Gameboy Camera, and probably other games).
- Added special handling for the [PHOTO! ROM](https://github.com/untoxa/gb-photo) for Gameboy Camera flashcarts. This ROM saves extra camera rolls to the cartridge's ROM. The original Burnmaster firmware only dumped the first 128 KB of ROM instead of the full 1 MB (which would be correct, normally). For the PHOTO ROM, this version of the firmware will dump the full 1 MB to include all 7 extra camera rolls. This can be imported directly to the [Game Boy Camera Gallery](https://github.com/HerrZatacke/gb-printer-web) or other apps. You will get a checksum error, but that's OK :)


## Building the firmware
**Requires [SEGGER Embedded Studio (6.22a recommended)](https://www.segger.com/products/development-tools/embedded-studio/)** 

To build the firmware open `GDCartReader.emProject` in Embedded Studio, then right click `Project 'GDCartReader'` and click `Build` the compiled firmware can be found at `ProjectFolder/Debug/Exe/GDCartReader.bin`, you will need to rename this file to `update.bin` which can now be put at the root of your SD Card to update your BurnMaster

**Alternatively** you can simply clone this repository and make your changes, Github will automatically build your firmware (See Actions Tab)

### Repository Notes:

This repository is setup to auto-build the firmware when a commit is made, builds can be found under the [Actions tab](https://github.com/HDR/Burnmaster-Firmware/actions), this makes it really easy to fork this repository and make your own changes without having to download Segger Embedded Studio


This source code is provided directly by funnyplaying, i have nothing to do with the development of the firmware.
