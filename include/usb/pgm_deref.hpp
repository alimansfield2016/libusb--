#pragma once

#include <usb/device.hpp>
#include <usb/configuration.hpp>
#include <usb/interface.hpp>
#include <usb/endpoint.hpp>

#include <avr/memory/pgmspace.hpp>

namespace AVR::USB
{
	
	constexpr AVR::pgm_ptr<uint8_t> getDeviceDescriptorBuf(AVR::pgm_ptr<Device> dev)
	{
		return dev->m_descriptor.ptr_pgm();
	}

	constexpr AVR::pgm_ptr<uint8_t> getConfigurationDescriptorBuf(AVR::pgm_ptr<Configuration> cfg)
	{
		return cfg->m_descriptor.ptr_pgm();
	}
	
	constexpr AVR::pgm_ptr<uint8_t> getInterfaceDescriptorBuf(AVR::pgm_ptr<Interface> itf)
	{
		return itf->m_descriptor.ptr_pgm();
	}
	
	constexpr AVR::pgm_ptr<uint8_t> getEndpointDescriptorBuf(const Endpoint *ept)
	{
		return ept->m_descriptor->ptr_pgm();
	}


	constexpr AVR::pgm_ptr<Configuration> getConfiguration(AVR::pgm_ptr<Device> dev, uint8_t idx = 0)
	{
		//if out of range return nullptr
		pgm_ptr ptr{&dev->m_configurations};
		auto configurations = *ptr;
		
		auto size{configurations.size()};
		if(idx >= size) return AVR::pgm_ptr<Configuration>{nullptr};

		AVR::pgm_ptr arr{configurations.begin()};
		if(arr)
			return arr+idx;
		return AVR::pgm_ptr<Configuration>{nullptr};
	}

	constexpr AVR::pgm_ptr<AVR::pgm_span<Interface>> getInterfaces(AVR::pgm_ptr<Configuration> dev, uint8_t idx = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr p_interfaces{&dev->m_interfaces};
		auto interfaces{*p_interfaces};
		
		auto size = interfaces.size();
		if(idx >= size) return AVR::pgm_ptr<AVR::pgm_span<Interface>>{nullptr};

		AVR::pgm_ptr arr{interfaces.begin()};
		if(arr)
			return arr+idx;
		return AVR::pgm_ptr<AVR::pgm_span<Interface>>{nullptr};
	}

	constexpr AVR::pgm_ptr<Interface> getInterface(AVR::pgm_ptr<Configuration> dev, uint8_t idx = 0, uint8_t alt = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr p_interfaces{getInterfaces(dev, idx)};
		if(!p_interfaces) return AVR::pgm_ptr<Interface>{nullptr};
		auto interfaces{*p_interfaces};

		auto size = interfaces.size();		
		if(alt >= size) return AVR::pgm_ptr<Interface>{nullptr};

		AVR::pgm_ptr arr{interfaces.begin()};
		if(arr)
			return arr+alt;
		return AVR::pgm_ptr<Interface>{nullptr};
	}

	constexpr AVR::USB::Endpoint* getEndpoint(AVR::pgm_ptr<Interface> itf, uint8_t idx = 0)
	{
		//if out of range, return nullptr
		pgm_span endpoints = *pgm_ptr{&itf->m_endpoints};
		// AVR::pgm_ptr size{endpoints->size_p()};
		auto size = endpoints.size();
		if(idx >= size) return nullptr;

		// AVR::pgm_ptr arr{*AVR::pgm_ptr{endpoints->p_ptr()}};
		AVR::pgm_ptr arr{endpoints.begin()};
		if(arr)
			return arr[idx];
		return nullptr;

	}

	constexpr AVR::USB::StringDescriptorTable::Str getString(const StringDescriptorTable *_tbl, uint8_t idx)
	{
		const StringDescriptorTable tbl = *AVR::pgm_ptr{_tbl};
		return tbl.string(idx);
	}

	constexpr AVR::pgm_ptr<AVR::USB::StringDescriptorTable> getStringTable(AVR::pgm_ptr<Device> dev, LanguageID /* lang */ = LanguageID::English_United_States)
	{
		AVR::pgm_ptr _ptr{&dev->m_strings};
		return _ptr;
	}
	
} // namespace AVR::USB
