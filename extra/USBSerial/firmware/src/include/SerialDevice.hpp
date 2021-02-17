#pragma once

#include <usb/device.hpp>
#include <usb/descriptor.hpp>
#include <usb/interface.hpp>
#include <usb/configuration.hpp>

#ifndef NUM_USBSERIAL
#define NUM_USBSERIAL 0
#endif

#define USBSERIAL_DEF(num, itfNo) \
extern const AVR::USB::EndpointDescriptor pgmSerial##num##In PROGMEM;	\
extern const AVR::USB::EndpointDescriptor pgmSerial##num##Out PROGMEM;	\
extern const AVR::pgm_array<AVR::USB::Endpoint*, 2> pgmEndpoints##itfNo PROGMEM; \
extern const AVR::USB::Interface pgmInterface##itfNo##Alt0 PROGMEM; \
extern const AVR::pgm_array<AVR::USB::Interface, 1> pgmInterface##itfNo PROGMEM;


#define CONFIG_DEF(n)\
extern const AVR::USB::Configuration pgmConfiguration0##n PROGMEM;\
extern const AVR::pgm_array<AVR::USB::Configuration, 1> pgmConfigurations##n PROGMEM



namespace AVR::USBSerial
{
	extern const AVR::pgm_array<AVR::USB::StringDescriptorTable::Str, 4> pgmStrings PROGMEM;
	extern const AVR::USB::StringDescriptorTable pgmStringTbl PROGMEM;

	USBSERIAL_DEF(0, 0)
	USBSERIAL_DEF(1, 1)
	USBSERIAL_DEF(2, 2)
	USBSERIAL_DEF(3, 3)
	USBSERIAL_DEF(4, 4)
	USBSERIAL_DEF(5, 5)
	USBSERIAL_DEF(6, 6)
	USBSERIAL_DEF(7, 7)
	USBSERIAL_DEF(8, 8)
	USBSERIAL_DEF(9, 9)
	USBSERIAL_DEF(10, 10)
	USBSERIAL_DEF(11, 11)
	USBSERIAL_DEF(12, 12)
	USBSERIAL_DEF(13, 13)
	USBSERIAL_DEF(14, 14)
	USBSERIAL_DEF(15, 15)

	extern const AVR::pgm_array<AVR::pgm_span<AVR::USB::Interface>, NUM_USBSERIAL> pgmInterfaces PROGMEM;

	extern const AVR::USB::Configuration pgmConfiguration0 PROGMEM;
	extern const AVR::pgm_array<AVR::USB::Configuration, 1> pgmConfigurations PROGMEM;

	extern const AVR::USB::Device pgmDevice PROGMEM;


} // namespace AVR::USBSerial
