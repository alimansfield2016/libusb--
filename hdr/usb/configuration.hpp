#pragma once

#include <usb/libusb++.hpp>
#include <array>

namespace AVR::USB
{
	class Interface;
	class Configuration
	{
	public:
		//Configuration string
		//Interfaces
		const std::constexpr_vector<
			const std::constexpr_vector<const Interface*>*> *m_interfaces;

		//PROGMEM
		const ConfigurationDescriptor m_descriptor;

		constexpr Configuration(
			ConfigurationAttributes _bmAttributes,
			const std::constexpr_vector<const std::constexpr_vector<const Interface*>*> *_interfaces,
			uint8_t _bConfigurationValue
		) : 
			m_interfaces{_interfaces},
			m_descriptor{
				static_cast<uint16_t>(0x0012),
				static_cast<uint8_t>(_interfaces->size()),
				static_cast<uint8_t>(_bConfigurationValue),
				static_cast<uint8_t>(0),	//iConfiguration
				_bmAttributes,
				100_mA
			}
			 {}

		constexpr std::constexpr_vector<const Interface*> getInterfacesPgm(uint8_t idx)
		{
			//PROGMEM
			AVR::pgm_ptr _ptr{m_interfaces};
			//not PROGMEM
			auto _v{*_ptr};
			//PROGMEM
			AVR::pgm_ptr arr{_v.begin()};
			//PROGMEM
			AVR::pgm_ptr ptr{arr[idx]};
			return *ptr;
		}
		constexpr std::constexpr_vector<const Interface*> getInterfacesPgmThisPgm(uint8_t idx)
		{
			//PROGMEM
			AVR::pgm_ptr _pptr{&m_interfaces};
			//PROGMEM
			AVR::pgm_ptr _ptr{*_pptr};
			//not PROGMEM
			auto _v{*_ptr};
			//PROGMEM
			AVR::pgm_ptr arr{_v.begin()};
			//PROGMEM
			AVR::pgm_ptr ptr{arr[idx]};
			return *ptr;
		}

		constexpr AVR::pgm_ptr<Interface> getInterfacePgm(uint8_t idx, uint8_t alt=0)
		{
			auto interfaces = getInterfacesPgm(idx);
			if(alt >= interfaces.size()) return AVR::pgm_ptr<Interface>{nullptr};
			AVR::pgm_ptr arr{interfaces.begin()};
			return AVR::pgm_ptr<Interface>{arr[alt]};
		}
		constexpr AVR::pgm_ptr<Interface> getInterfacePgmThisPgm(uint8_t idx, uint8_t alt=0)
		{
			auto interfaces = getInterfacesPgmThisPgm(idx);
			if(alt >= interfaces.size()) return AVR::pgm_ptr<Interface>{nullptr};
			AVR::pgm_ptr arr{interfaces.begin()};
			return AVR::pgm_ptr<Interface>{arr[alt]};
		}

		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgm() 
		{
			AVR::pgm_ptr buf{m_descriptor.m_ptr};
			return buf;
		}
		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgmThisPgm() 
		{
			AVR::pgm_ptr ptr{&m_descriptor.m_ptr};
			AVR::pgm_ptr buf{*ptr};
			return buf;
		}
	};
} // namespace AVR::USB
