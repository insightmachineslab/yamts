#!/bin/bash

# Assuming Arduino is installed under /opt/arduino:
arduinoHome='/opt/arduino'

mkdir -p bin
pwd=`pwd`
inoFile=$(ls src/*.ino | head -n 1)
$arduinoHome/arduino-builder -compile -logger=human -hardware $arduinoHome/hardware -hardware $HOME/.arduino15/packages -tools $arduinoHome/tools-builder -tools $arduinoHome/hardware/tools/avr -tools $HOME/.arduino15/packages -built-in-libraries $arduinoHome/libraries -libraries $HOME/Arduino/libraries -fqbn=adafruit:samd:adafruit_feather_m0 -ide-version=10807 -build-path $pwd/bin -build-cache $pwd/bin -warnings=default -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.bossac.path=$HOME/.arduino15/packages/arduino/tools/bossac/1.8.0-48-gb176eee -prefs=runtime.tools.bossac-1.8.0-48-gb176eee.path=$HOME/.arduino15/packages/arduino/tools/bossac/1.8.0-48-gb176eee -prefs=runtime.tools.arm-none-eabi-gcc.path=$HOME/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1 -prefs=runtime.tools.arm-none-eabi-gcc-4.8.3-2014q1.path=$HOME/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1 -prefs=runtime.tools.CMSIS.path=$HOME/.arduino15/packages/arduino/tools/CMSIS/4.5.0 -prefs=runtime.tools.CMSIS-4.5.0.path=$HOME/.arduino15/packages/arduino/tools/CMSIS/4.5.0 -prefs=runtime.tools.openocd.path=$HOME/.arduino15/packages/arduino/tools/openocd/0.9.0-arduino -prefs=runtime.tools.openocd-0.9.0-arduino.path=$HOME/.arduino15/packages/arduino/tools/openocd/0.9.0-arduino -prefs=runtime.tools.CMSIS-Atmel.path=$HOME/.arduino15/packages/arduino/tools/CMSIS-Atmel/1.2.0 -prefs=runtime.tools.CMSIS-Atmel-1.2.0.path=$HOME/.arduino15/packages/arduino/tools/CMSIS-Atmel/1.2.0 -verbose $inoFile

