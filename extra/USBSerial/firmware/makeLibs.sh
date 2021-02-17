#!/bin/sh

range=$(seq 1 15)
if [ $# -gt 0 ]
then
	if [ $1 = "clean" ]
	then
		echo "$1"
		clean=1
	else if [ $1 = "help" ]
		echo "$0: makes usbSerial libraries with specified number of serial ports"
		echo "clean: deletes all libraries"
		echo "help: shows this menu"
		echo "n.. : list numbers of serial-ports to build libs for"
	else
		clean=0
		range=$@
	fi
fi
for i in $range
do
	libName="libUSBSerial$i.a"
	echo $libName
	rm -f $libName
	if [ $clean -eq 0 ]
	then
		make clean lib CUSTOM_DEFINES+="NUM_USBSERIAL=$i"
		mv libUSBSerial.a libUSBSerial$i.a
	fi
done
