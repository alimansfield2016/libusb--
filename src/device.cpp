
#include <libusb++.hpp>
#include <device.hpp>
#include <configuration.hpp>
#include <interface.hpp>

#include <array>

constexpr std::constexpr_vector_c<AVR::USB::Endpoint*, 0> PROGMEM pgmEndpoints{}; 

constexpr AVR::USB::Interface PROGMEM pgmInterface{
	AVR::USB::InterfaceClass::VendorSpecific,
	0x00,
	0x01,
	&pgmEndpoints,
	0,
	0
};

constexpr std::constexpr_vector_c<const AVR::USB::Interface*, 1> PROGMEM pgmInterfaceConfiguration{{
	&pgmInterface
}};
constexpr std::constexpr_vector_c<const std::constexpr_vector<const AVR::USB::Interface*>*, 1> PROGMEM pgmInterfaceConfigurations{{
	&pgmInterfaceConfiguration
}};

constexpr AVR::USB::Configuration PROGMEM pgmConfiguration{
	AVR::USB::ConfigurationAttributes::NONE,
	&pgmInterfaceConfigurations,
	0
};

constexpr std::constexpr_vector_c<const AVR::USB::Configuration*, 1> PROGMEM pgmConfigurations{{
	&pgmConfiguration
}};

constexpr AVR::USB::Device PROGMEM pgmDevice{
	AVR::USB::USB_BCD::USB1_1,
	0x16C0,
	0x05DC,
	0x0104,
	AVR::USB::DeviceClass::VendorSpecific,
	0x00,
	0x00,
	&pgmConfigurations
};

const AVR::USB::Device *AVR::USB::pDevice{&pgmDevice};
