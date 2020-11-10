#pragma once

#include <usb/device.hpp>
#include <usb/configuration.hpp>
#include <usb/interface.hpp>
#include <usb/endpoint.hpp>

#include <avr/memory/pgmspace.hpp>

namespace AVR::USB
{
	
	constexpr AVR::pgm_ptr<uint8_t> getDeviceDescriptorBuf(const Device *dev)
	{
		return dev->m_descriptor.ptr_pgm();
	}

	constexpr AVR::pgm_ptr<uint8_t> getConfigurationDescriptorBuf(const Configuration *cfg)
	{
		return cfg->m_descriptor.ptr_pgm();
	}
	
	constexpr AVR::pgm_ptr<uint8_t> getInterfaceDescriptorBuf(const Interface *itf)
	{
		return itf->m_descriptor.ptr_pgm();
	}
	
	constexpr AVR::pgm_ptr<uint8_t> getEndpointDescriptorBuf(const Endpoint *ept)
	{
		return ept->m_descriptor->ptr_pgm();
	}


	constexpr const Configuration* getConfiguration(const Device *dev, uint8_t idx = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr p_configurations{&dev->m_configurations};
		AVR::pgm_ptr configurations{*p_configurations};
		
		AVR::pgm_ptr size{configurations->size_p()};
		if(idx >= *size) return nullptr;

		// AVR::pgm_ptr arr{*AVR::pgm_ptr{configurations->p_ptr()}};
		AVR::pgm_ptr arr{configurations->begin()};
		if(arr)
			return arr[idx];
		return nullptr;
	}

	constexpr AVR::pgm_ptr<std::constexpr_vector<const Interface*>> getInterfaces(const Configuration *dev, uint8_t idx = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr p_interfaces{&dev->m_interfaces};
		AVR::pgm_ptr interfaces{*p_interfaces};
		
		AVR::pgm_ptr size{interfaces->size_p()};
		if(idx >= *size) return nullptr;

		// AVR::pgm_ptr arr{*AVR::pgm_ptr{interfaces->p_ptr()}};
		AVR::pgm_ptr arr{interfaces->begin()};
		if(arr)
			return arr[idx];
		return nullptr;
	}

	constexpr const Interface* getInterface(const Configuration *dev, uint8_t idx = 0, uint8_t alt = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr p_interfaces{&dev->m_interfaces};
		AVR::pgm_ptr interfaces{*p_interfaces};

		AVR::pgm_ptr size{interfaces->size_p()};
		
		if(idx >= *size) return nullptr;

		// AVR::pgm_ptr arr{*AVR::pgm_ptr{interfaces->p_ptr()}};
		AVR::pgm_ptr arr{interfaces->begin()};
		if(!arr) return nullptr;

		AVR::pgm_ptr elem{arr[idx]};

		size = elem->size_p();
		if(alt >= *size) return nullptr;

		// AVR::pgm_ptr p_interface{*AVR::pgm_ptr{elem->p_ptr()}};
		AVR::pgm_ptr p_interface{elem->begin()};
		
		return p_interface[idx];
	}

	constexpr AVR::USB::Endpoint* getEndpoint(const Interface *itf, uint8_t idx = 0)
	{
		//if out of range, return nullptr
		AVR::pgm_ptr<const std::constexpr_vector<Endpoint*>*> p_endpoints{&itf->m_endpoints};
		AVR::pgm_ptr<const std::constexpr_vector<Endpoint*>> endpoints{*p_endpoints};
		// const std::constexpr_vector<AVR::USB::Endpoint *> _arr{*endpoints};
		
		AVR::pgm_ptr size{endpoints->size_p()};
		if(idx >= *size) return nullptr;

		// AVR::pgm_ptr arr{*AVR::pgm_ptr{endpoints->p_ptr()}};
		AVR::pgm_ptr arr{endpoints->begin()};
		if(arr)
			return arr[idx];
		return nullptr;

	}

	constexpr AVR::USB::StringDescriptorTable::Str getString(const StringDescriptorTable *_tbl, uint8_t idx)
	{
		const StringDescriptorTable tbl = *AVR::pgm_ptr{_tbl};
		return tbl.string(idx);
	}

	constexpr const AVR::USB::StringDescriptorTable* getStringTable(const Device *dev, LanguageID /* lang */ = LanguageID::English_United_States)
	{
		AVR::pgm_ptr<const AVR::USB::StringDescriptorTable*> _ptr{&dev->m_strings};
		return *_ptr;
	}
	
} // namespace AVR::USB
