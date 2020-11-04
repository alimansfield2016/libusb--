#pragma once

#include <usb/libusb++.hpp>
#include <usb/descriptor.hpp>
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

		constexpr std::constexpr_vector<Endpoint*> getEndpointsPgm()
		{
			AVR::pgm_ptr _v{m_endpoints};
			auto v{*_v};
			return v;
		}
		constexpr std::constexpr_vector<Endpoint*> getEndpointsPgmThisPgm()
		{
			AVR::pgm_ptr _ptr{&m_endpoints};
			AVR::pgm_ptr _v{*_ptr};
			auto v{*_v};
			return v;
		}

		constexpr Endpoint* getEndpoint(uint8_t idx)
		{
			auto endpoints = getEndpointsPgm();
			if(idx >= endpoints.size()) return nullptr;
			AVR::pgm_ptr arr{endpoints.begin()};
			return arr[idx];
		}
		constexpr Endpoint* getEndpointPgmThis(uint8_t idx)
		{
			auto endpoints = getEndpointsPgmThisPgm();
			if(idx >= endpoints.size()) return nullptr;
			AVR::pgm_ptr arr{endpoints.begin()};
			return arr[idx];
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