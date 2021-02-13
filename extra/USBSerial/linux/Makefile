obj-m = usbserial_generic_lowspeed.o
KVERSION = $(shell uname -r)

all: module

module:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

load: module
	sudo insmod usbserial_generic_lowspeed.ko
unload:
	sudo rmmod usbserial_generic_lowspeed