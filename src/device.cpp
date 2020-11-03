
#include <libusb++.hpp>
#include <descriptor.hpp>
#include <device.hpp>
#include <configuration.hpp>
#include <interface.hpp>

#include <array>

const wchar_t PROGMEM _MfrStr[] = L"www.fischl.de";
const wchar_t PROGMEM _ProdStr[] = L"USBasp";
const wchar_t PROGMEM _SerStr[] = L"AM_USB";

#define STR(str) sizeof(str)-2, str

using Str = AVR::USB::StringDescriptorTable::Str;

const Str PROGMEM MfrStr{STR(_MfrStr)};
const Str PROGMEM ProdStr{STR(_ProdStr)};
const Str PROGMEM SerStr{STR(_SerStr)};

const std::constexpr_vector_c<const Str*, 3> PROGMEM pgmStringsEN_US
{{
	&MfrStr,
	&ProdStr,
	&SerStr,
}};

const AVR::USB::StringDescriptorTable PROGMEM pgmStringTable
{
	AVR::USB::LanguageID::English_United_States,
	&pgmStringsEN_US
};

const std::constexpr_vector_c<AVR::USB::Endpoint*, 0> PROGMEM pgmEndpoints{}; 

const AVR::USB::Interface PROGMEM pgmInterface{
	AVR::USB::InterfaceClass::NONE,
	0x00,
	0x01,
	&pgmEndpoints,
	0,
	0
};

const std::constexpr_vector_c<const AVR::USB::Interface*, 1> PROGMEM pgmInterfaceConfiguration{{
	&pgmInterface
}};
const std::constexpr_vector_c<const std::constexpr_vector<const AVR::USB::Interface*>*, 1> PROGMEM pgmInterfaceConfigurations{{
	&pgmInterfaceConfiguration
}};

const AVR::USB::Configuration PROGMEM pgmConfiguration{
	AVR::USB::ConfigurationAttributes::NONE,
	&pgmInterfaceConfigurations,
	1
};

const std::constexpr_vector_c<const AVR::USB::Configuration*, 1> PROGMEM pgmConfigurations{{
	&pgmConfiguration
}};

const AVR::USB::Device PROGMEM pgmDevice{
	AVR::USB::USB_BCD::USB1_1,
	0x16C0,
	0x05DC,
	0x0104,
	AVR::USB::DeviceClass::VendorSpecific,
	0x00,
	0x00,
	&pgmConfigurations,
	&pgmStringTable,
	1,
	2,
	3,
};

const AVR::USB::Device *AVR::USB::pDevice{&pgmDevice};
