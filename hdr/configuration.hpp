#pragma once

#include <libusb++.hpp>
#include <array>

namespace AVR::USB
{
	class Interface;
	class Configuration
	{
		//Configuration string
		//Interfaces
		const std::constexpr_vector<
			const std::constexpr_vector<const Interface*>*> *m_interfaces;

		//PROGMEM
		const ConfigurationDescriptor m_descriptor;

	public:
		constexpr Configuration(
			ConfigurationAttributes _bmAttributes,
			const std::constexpr_vector<const std::constexpr_vector<const Interface*>*> *_interfaces,
			uint8_t _bConfigurationValue
		) : 
			m_interfaces{_interfaces},
			m_descriptor{
				0x0000,
				static_cast<uint8_t>(_interfaces->size()),
				_bConfigurationValue,
				0,	//iConfiguration
				_bmAttributes,
				100_mA
			}
			 {}
	};
} // namespace AVR::USB
