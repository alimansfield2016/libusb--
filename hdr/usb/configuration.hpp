#pragma once

#include <usb/libusb++.hpp>
#include <usb/interface.hpp>
#include <array>

namespace AVR::USB
{

	/**
	 * 
	 *	ConfigurationAttributes _bmAttributes
	 *	const std::constexpr_vector_c<const std::constexpr_vector_c<const Interface*, m>*, n> *_interfaces
	 *	uint8_t _bConfigurationValue
	 * 
	 */
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
			uint8_t _bConfigurationValue,
			uint8_t _iConfiguration = 0
		) : 
			m_interfaces{_interfaces},
			m_descriptor{
				static_cast<uint16_t>(totalBufSize()),
				// static_cast<uint16_t>(0),
				static_cast<uint8_t>(_interfaces->size()),
				// static_cast<uint8_t>(0),
				static_cast<uint8_t>(_bConfigurationValue),
				static_cast<uint8_t>(_iConfiguration),
				_bmAttributes,
				100_mA
			}
			 {}

		constexpr AVR::pgm_ptr<std::constexpr_vector<const Interface*>> getInterfacesPgm(uint8_t idx)
		{
			//PROGMEM
			AVR::pgm_ptr _ptr{m_interfaces};
			//not PROGMEM
			// auto _v{*_ptr};
			//PROGMEM
			AVR::pgm_ptr size{_ptr->size_p()};
			if(idx >= *size) return nullptr;
			AVR::pgm_ptr arr{*AVR::pgm_ptr{_ptr->p_ptr()}};
			// AVR::pgm_ptr arr{_ptr->begin()};
			//PROGMEM
			AVR::pgm_ptr ptr{arr[idx]};
			return ptr;
		}
		constexpr AVR::pgm_ptr<std::constexpr_vector<const Interface*>> getInterfacesPgmThisPgm(uint8_t idx)
		{
			//PROGMEM
			AVR::pgm_ptr _pptr{&m_interfaces};
			//PROGMEM
			AVR::pgm_ptr _ptr{*_pptr};
			//not PROGMEM
			// auto _v{*_ptr};
			//PROGMEM
			AVR::pgm_ptr size{_ptr->size_p()};
			if(idx >= *size) return nullptr;
			AVR::pgm_ptr arr{*AVR::pgm_ptr{_ptr->p_ptr()}};
			// AVR::pgm_ptr arr{_ptr->begin()};
			//PROGMEM
			AVR::pgm_ptr ptr{arr[idx]};
			return ptr;
		}

		constexpr AVR::pgm_ptr<Interface> getInterfacePgm(uint8_t idx, uint8_t alt=0)
		{
			auto interfaces = getInterfacesPgm(idx);
			AVR::pgm_ptr size{interfaces->size_p()};
			if(alt >= *size) return nullptr;
			AVR::pgm_ptr arr{*AVR::pgm_ptr{interfaces->p_ptr()}};
			// AVR::pgm_ptr arr{interfaces->begin()};
			return AVR::pgm_ptr<Interface>{arr[alt]};
		}
		constexpr AVR::pgm_ptr<Interface> getInterfacePgmThisPgm(uint8_t idx, uint8_t alt=0)
		{
			auto interfaces = getInterfacesPgmThisPgm(idx);
			AVR::pgm_ptr size{interfaces->size_p()};
			if(alt >= *size) return nullptr;
			AVR::pgm_ptr arr{*AVR::pgm_ptr{interfaces->p_ptr()}};
			// AVR::pgm_ptr arr{interfaces->begin()};
			return AVR::pgm_ptr<Interface>{arr[alt]};
		}

		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgm() 
		{
			return m_descriptor.ptr();
			// AVR::pgm_ptr buf{m_descriptor.m_ptr};
			// return buf;
		}
		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgmThisPgm() 
		{
			return m_descriptor.ptr_pgm();
			// AVR::pgm_ptr ptr{&m_descriptor.m_ptr};
			// AVR::pgm_ptr buf{*ptr};
			// return buf;
		}

	private:
		constexpr uint16_t totalBufSize() const
		{
			uint16_t total = ConfigurationDescriptor::s_size;
			for(auto _it1 : *m_interfaces)
				for(auto _it2 : *_it1)
					total += _it2->totalBufSize();
			return total;
		}
	};
} // namespace AVR::USB
