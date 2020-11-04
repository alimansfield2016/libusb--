#pragma once

#include <usb/libusb++.hpp>
#include <usb/descriptor.hpp>
#include <array>

namespace AVR::USB
{
	class Configuration;
	class Device
	{
	public:

		//Configurations
		const std::constexpr_vector<const Configuration*> *m_configurations;

		//Manufacturer string
		//Product string
		//Serial number string
		const StringDescriptorTable *m_strings;

		const DeviceDescriptor m_descriptor;

		constexpr Device(
			USB_BCD _bcdUSB,
			uint16_t _idVendor,
			uint16_t _idProduct,
			uint16_t _bcdDevice,
			DeviceClass _bDeviceClass,
			uint8_t _bSubDeviceClass,
			uint8_t _bDeviceProtocol,

			const std::constexpr_vector<const Configuration*> *_configurations,
			const StringDescriptorTable *_strings,
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
				static_cast<uint8_t>(_configurations->size())
			}
			 {}
		constexpr std::constexpr_vector<const Configuration*> getConfigurationsPgm() const {
			return *AVR::pgm_ptr{m_configurations};
		}
		constexpr std::constexpr_vector<const Configuration*> getConfigurationsPgmThisPgm() const {
			AVR::pgm_ptr ptr{&m_configurations};
			return *AVR::pgm_ptr{*ptr};
		}

		constexpr AVR::pgm_ptr<Configuration> getConfigurationPgm(uint8_t idx) const
		{
			//not PROGMEM
			auto config = getConfigurationsPgm();
			if(idx >= config.size()) return AVR::pgm_ptr<Configuration>{nullptr};
			//PROGMEM
			AVR::pgm_ptr arr{config.begin()};
			return AVR::pgm_ptr<Configuration>{arr[idx]};
		}
		constexpr AVR::pgm_ptr<Configuration> getConfigurationPgmThisPgm(uint8_t idx) const
		{
			//not PROGMEM
			auto config = getConfigurationsPgmThisPgm();
			if(idx >= config.size()) return AVR::pgm_ptr<Configuration>{nullptr};
			//PROGMEM
			AVR::pgm_ptr arr{config.begin()};
			return AVR::pgm_ptr<Configuration>{arr[idx]};
		}

		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgm() const 
		{
			AVR::pgm_ptr buf{m_descriptor.m_ptr};
			return buf;
		}
		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgmThisPgm() const 
		{
			AVR::pgm_ptr ptr{&m_descriptor.m_ptr};
			AVR::pgm_ptr buf{*ptr};
			return buf;
		}
	
		constexpr AVR::pgm_ptr<StringDescriptorTable> getStringTablePgmThisPgm() const
		{
			AVR::pgm_ptr _ptr{&m_strings};
			AVR::pgm_ptr str_tbl{*_ptr};
			return str_tbl;
		}
	};

} // namespace AVR::USB
