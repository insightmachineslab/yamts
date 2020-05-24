#!/bin/bash
usbDevice=$(ls /dev/ttyACM* | head -n 1)
binFile=$(ls bin/*.bin | head -n 1)
echo "Uploading" $binFile "to" $usbDevice
~/.arduino15/packages/arduino/tools/bossac/1.7.0/bossac -i -d --port=$usbDevice -U true -i -e -w -v $binFile -R 

echo 
echo "Waiting for" $usbDevice "to reappear..."

sleep 2

echo 
echo "Listening to " $usbDevice
picocom -b 9600 $usbDevice
