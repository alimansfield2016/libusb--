#pragma once

#include <libusb++.hpp>
#include <array>
#include <descriptor.hpp>

namespace AVR::USB
{
	class Configuration;
	class Device
	{

		//Manufacturer string
		//Product string
		//Serial number string

		//Configurations
		const std::constexpr_vector<const Configuration*> *m_configurations;

		const DeviceDescriptor m_descriptor;

	public:
		constexpr Device(
			USB_BCD _bcdUSB,
			uint16_t _idVendor,
			uint16_t _idProduct,
			uint16_t _bcdDevice,
			DeviceClass _bDeviceClass,
			uint8_t _bSubDeviceClass,
			uint8_t _bDeviceProtocol,

			const std::constexpr_vector<const Configuration*> *_configurations
		) : 
			m_configurations{_configurations},
			m_descriptor{
				_bcdUSB,
				_bDeviceClass,
				_bSubDeviceClass,
				_bDeviceProtocol,
				8,
				_idVendor,
				_idProduct,
				_bcdDevice,
				0, 0, 0, 
				static_cast<uint8_t>(_configurations->size())
			}
			 {}

	};

} // namespace AVR::USB
