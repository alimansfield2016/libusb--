#pragma once

#include <usb/libusb++.hpp>
#include <usb/descriptor.hpp>

namespace AVR::USB
{
	class Interface;

	using AltInterfaces = AVR::pgm_span<const Interface*>;
	using Interfaces = AVR::pgm_span<const AltInterfaces*>;
	/**
	 * Interface Class Arguments
	 * 
	 * InterfaceClass _bInterfaceClass
	 * uint8_t _bInterfaceSubClass
	 * uint8_t _bInterfaceProtocol
	 * const std::constexpr_vector<Endpoint*, n> *_endpoints
	 * uint8_t _bInterfaceNumber
	 * uint8_t _bAlternateSetting
	 * 
	 */
	class Interface
	{
		friend class Configuration;
	public:
		//Endpoints
		const AVR::pgm_span<Endpoint*> m_endpoints;

		//descriptor
		InterfaceDescriptor m_descriptor;

		constexpr Interface(
			InterfaceClass _bInterfaceClass,
			uint8_t _bInterfaceSubClass,
			uint8_t _bInterfaceProtocol,
			AVR::pgm_span<Endpoint*> _endpoints,
			uint8_t _bInterfaceNumber,
			uint8_t _bAlternateSetting,
			uint8_t _iInterface = 0
		) : 
			m_endpoints{_endpoints},
			m_descriptor{
				_bInterfaceNumber,
				_bAlternateSetting,
				static_cast<uint8_t>(_endpoints.size()),
				_bInterfaceClass,
				_bInterfaceSubClass,
				_bInterfaceProtocol,
				// _InterfaceStr->idx(),
				_iInterface
			} 
			{}

		constexpr AVR::pgm_span<Endpoint*> getEndpointsPgm()
		{
			return m_endpoints;
		}
		constexpr AVR::pgm_span<Endpoint*> getEndpointsPgmThisPgm()
		{
			auto endpoints = *pgm_ptr{&m_endpoints};
			return endpoints;
		}

		constexpr Endpoint* getEndpoint(uint8_t idx)
		{
			auto endpoints = getEndpointsPgm();
			// AVR::pgm_ptr size{endpoints->size_p()};
			auto size = endpoints.size();
			if(idx >= size) return nullptr;
			AVR::pgm_ptr arr{endpoints.begin()};
			return arr[idx];
		}
		constexpr Endpoint* getEndpointPgmThis(uint8_t idx)
		{
			auto endpoints = getEndpointsPgmThisPgm();
			// AVR::pgm_ptr size{endpoints->size_p()};
			auto size = endpoints.size();
			if(idx >= size) return nullptr;
			AVR::pgm_ptr arr{endpoints.begin()};
			return arr[idx];
		}

		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgm() 
		{
			return AVR::pgm_ptr{m_descriptor.ptr()};
		}
		constexpr AVR::pgm_ptr<uint8_t> getDescriptorBufPgmThisPgm() 
		{
			return m_descriptor.ptr_pgm();
		}

	private:
		constexpr uint16_t totalBufSize() const
		{
			uint16_t total = InterfaceDescriptor::s_size;
			total += m_endpoints.size() * EndpointDescriptor::s_size;
			return total;
		}
	
	};
} // namespace AVR::USB
