#pragma once

#include <libusb++.hpp>
#include <descriptor.hpp>
#include <array>

namespace AVR::USB
{
	class Interface;
	class Interface
	{
	public:
		//Endpoints
		const std::constexpr_vector<Endpoint*> *m_endpoints;

		//descriptor
		const InterfaceDescriptor m_descriptor;

		constexpr Interface(
			InterfaceClass _bInterfaceClass,
			uint8_t _bInterfaceSubClass,
			uint8_t _bInterfaceProtocol,
			const std::constexpr_vector<Endpoint*> *_endpoints,
			uint8_t _bInterfaceNumber,
			uint8_t _bAlternateSetting
		) : 
			m_endpoints{_endpoints},
			m_descriptor{
				_bInterfaceNumber,
				_bAlternateSetting,
				static_cast<uint8_t>(_endpoints->size()),
				_bInterfaceClass,
				_bInterfaceSubClass,
				_bInterfaceProtocol,
				// _InterfaceStr->idx(),
				0
			} {}
	};
} // namespace AVR::USB
