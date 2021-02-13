#pragma once

#include <usb/libusb++.hpp>
#include <usb/descriptor.hpp>
#include <array>
#include <usb/configuration.hpp>

namespace AVR::USB
{
	class Configuration;
	class Device
	{
	public:

		//Configurations
		AVR::pgm_span<Configuration> m_configurations;

		//Manufacturer string
		//Product string
		//Serial number string
		//etc
		StringDescriptorTable m_strings;

		DeviceDescriptor m_descriptor;

		constexpr Device(
			USB_BCD _bcdUSB,
			uint16_t _idVendor,
			uint16_t _idProduct,
			uint16_t _bcdDevice,
			DeviceClass _bDeviceClass,
			uint8_t _bSubDeviceClass,
			uint8_t _bDeviceProtocol,

			AVR::pgm_span<Configuration> _configurations,
			StringDescriptorTable _strings,
			uint8_t _iManufacturer = 0,
			uint8_t _iProduct = 0,
			uint8_t _iSerialNumber = 0
		) : 
			m_configurations{_configurations},
			m_strings{_strings},
			m_descriptor{
				_bcdUSB,
				_bDeviceClass,
				_bSubDeviceClass,
				_bDeviceProtocol,
				8,
				_idVendor,
				_idProduct,
				_bcdDevice,
				_iManufacturer,
				_iProduct,
				_iSerialNumber,
				static_cast<uint8_t>(_configurations.size())
			}
			 {}
		constexpr AVR::pgm_span<Configuration> getConfigurationsPgm() const {
			return m_configurations;
		}
		constexpr AVR::pgm_span<Configuration> getConfigurationsPgmThisPgm() const {
			AVR::pgm_ptr ptr{&m_configurations};
			return *ptr;
		}

		constexpr AVR::pgm_ptr<Configuration> getConfigurationPgm(uint8_t idx) const
		{
			//PROGMEM
			auto config = getConfigurationsPgm();
			if(idx >= config.size()) return AVR::pgm_ptr<Configuration>{nullptr};
			//PROGMEM
			AVR::pgm_ptr arr{config.begin()};
			return arr+idx;
		}
		constexpr AVR::pgm_ptr<Configuration> getConfigurationPgmThisPgm(uint8_t idx) const
		{
			//PROGMEM
			auto config = getConfigurationsPgmThisPgm();
			if(idx >= config.size()) return AVR::pgm_ptr<Configuration>{nullptr};
			//PROGMEM
			AVR::pgm_ptr arr{config.begin()};
			return arr+idx;
		}

		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgm() const 
		{
			return AVR::pgm_ptr{m_descriptor.ptr()};
		}
		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgmThisPgm() const 
		{
			return m_descriptor.ptr_pgm();
		}
	
		constexpr AVR::pgm_ptr<StringDescriptorTable> getStringTablePgmThisPgm() const
		{
			AVR::pgm_ptr str_tbl{&m_strings};
			return str_tbl;
		}
	};

} // namespace AVR::USB
