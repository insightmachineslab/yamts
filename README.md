# YAMTS
Device and client code for the YAMTS project

## Client
The "client" directory contains the source files for YAMTS-Control application. 
The application is based on the Apache Cordova platform, so you will need to install it before building the project. 
Follow the instructions [here](https://cordova.apache.org/docs/en/latest/guide/cli/index.html) to install and configure Cordova for working with Android devices.
When running the application on your smartphone, be sure to connect to the WiFi access point that's running on the YAMTS device.
You will need to provide a password that is specified in the "secrets.h" file in the device's source code.

## Device
The "device" directory contains the source code and useful scripts for the YAMTS firmware. 
The source code is in the "src" directory. 
The main file is called Main.ino but that does not mean that this is an Arduino project. 
The code can be built using the Arduino-Builder tool. 
It is installed along with Arduino but is a command line tool so can be easilly invoked from a script.
And that's exactly how this project was originally built.
The "build.sh" script calls the Arduino-Builder with all the necessary arguments.
Then the "upload.sh" script can be used to upload the .bin file to the device using the bossac tool (also provided by Arduino).

If you want to use the Arduino IDE, just move all the files from the "src" directory to the "device" directory and rename the Main.ino file to device.ino.
You will not need the .sh scripts anymore.

### Dependencies
There is only one library that needs to be installed via the Arduino Library Manager: the webthing-arduino library (along with its dependencies).

### WiFi password
The RemoteConfigurationController.h references a "secrets.h" file that contains only two lines:
```
static const char SSID[] = "xxx";
static const char PASSWORD[] = "xxx";
```
This file is missing from the repository so you should create one and provide whatever values you want to use for the SSID and PASSWORD.

