#pragma once

#include <usb/libusb++.hpp>
#include <usb/interface.hpp>
#include <array>

namespace AVR::USB
{

	/**
	 * 
	 *	ConfigurationAttributes _bmAttributes
	 *	const std::constexpr_vector<const std::constexpr_vector<const Interface*, m>*, n> *_interfaces
	 *	uint8_t _bConfigurationValue
	 * 
	 */
	class Configuration
	{
	public:
		//Interfaces
		const AVR::pgm_span<AVR::pgm_span<Interface>> m_interfaces;

		//PROGMEM
		const ConfigurationDescriptor m_descriptor;

		constexpr Configuration(
			ConfigurationAttributes _bmAttributes,
			AVR::pgm_span<AVR::pgm_span<Interface>> _interfaces,
			uint8_t _bConfigurationValue,
			uint8_t _iConfiguration = 0
		) : 
			m_interfaces{_interfaces},
			m_descriptor{
				static_cast<uint16_t>(totalBufSize()),
				static_cast<uint8_t>(_interfaces.size()),
				static_cast<uint8_t>(_bConfigurationValue),
				static_cast<uint8_t>(_iConfiguration),
				_bmAttributes,
				100_mA
			}
			 {}

		constexpr AVR::pgm_ptr<AVR::pgm_span<Interface>> getInterfacesPgm(uint8_t idx)
		{
			auto size{m_interfaces.size()};
			if(idx >= size) return AVR::pgm_ptr<AVR::pgm_span<Interface>>{nullptr};
			//PROGMEM
			AVR::pgm_ptr arr{m_interfaces.begin()};
			AVR::pgm_ptr ptr{arr+idx};
			return ptr;
		}
		constexpr AVR::pgm_ptr<AVR::pgm_span<Interface>> getInterfacesPgmThisPgm(uint8_t idx)
		{
			//PROGMEM
			auto _interfaces = *pgm_ptr{&m_interfaces};

			auto size{_interfaces.size()};
			if(idx >= size) return AVR::pgm_ptr<AVR::pgm_span<Interface>>{nullptr};
			//PROGMEM
			AVR::pgm_ptr arr{_interfaces.begin()};
			AVR::pgm_ptr ptr{arr+idx};
			return ptr;
		}

		constexpr AVR::pgm_ptr<Interface> getInterfacePgm(uint8_t idx, uint8_t alt=0)
		{
			auto interface = *getInterfacesPgm(idx);
			auto size = interface.size();
			if(alt >= size) return AVR::pgm_ptr<Interface>{nullptr};
			AVR::pgm_ptr arr{interface.begin()};
			return arr+alt;
		}
		constexpr AVR::pgm_ptr<Interface> getInterfacePgmThisPgm(uint8_t idx, uint8_t alt=0)
		{
			auto interface = *getInterfacesPgmThisPgm(idx);
			auto size = interface.size();
			if(alt >= size) return AVR::pgm_ptr<Interface>{nullptr};
			AVR::pgm_ptr arr{interface.begin()};
			return arr+alt;
		}

		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgm() 
		{
			return AVR::pgm_ptr<uint8_t>{m_descriptor.ptr()};
		}
		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgmThisPgm() 
		{
			return m_descriptor.ptr_pgm();
		}

	private:
	/**
	 * @brief VERY HACKY, please ignore
	 * this hacks the compiler constexpr engine to work with PROGMEM objects.
	 * 
	 * @return constexpr uint16_t 
	 */
		constexpr uint16_t totalBufSize() const
		{
			uint16_t total = ConfigurationDescriptor::s_size;
			for(auto _it1 = m_interfaces.begin().get(); _it1 != m_interfaces.end().get(); ++_it1)
				for(auto _it2 = _it1->begin().get(); _it2 != _it1->end().get(); _it2++)
					total += _it2->totalBufSize();
			return total;
		}
	};
} // namespace AVR::USB
