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
		AVR::pgm_ptr<const uint8_t*> p_buf{&dev->m_descriptor.m_ptr};
		return AVR::pgm_ptr<uint8_t>{*p_buf};
	}

	constexpr AVR::pgm_ptr<uint8_t> getConfigurationDescriptorBuf(const Configuration *cfg)
	{
		AVR::pgm_ptr<const uint8_t*> p_buf{&cfg->m_descriptor.m_ptr};
		return AVR::pgm_ptr<uint8_t>{*p_buf};
	}
	
	constexpr AVR::pgm_ptr<uint8_t> getInterfaceDescriptorBuf(const Interface *itf)
	{
		AVR::pgm_ptr<const uint8_t*> p_buf{&itf->m_descriptor.m_ptr};
		return AVR::pgm_ptr<uint8_t>{*p_buf};
	}
	
	constexpr AVR::pgm_ptr<uint8_t> getEndpointDescriptorBuf(const Endpoint *ept)
	{
		// AVR::pgm_ptr<const EndpointDescriptor> p_desc{ept->m_descriptor};
		// AVR::pgm_ptr<const uint8_t> p_buf{*p_desc.m_ptr};
		// return AVR::pgm_ptr<const uint8_t>{*p_buf};
		return AVR::pgm_ptr<uint8_t>(nullptr);
	}


	constexpr const Configuration* getConfiguration(const Device *dev, uint8_t idx = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr<const std::constexpr_vector<const Configuration*>*> p_configurations{&dev->m_configurations};
		AVR::pgm_ptr<const std::constexpr_vector<const Configuration*>> configurations{*p_configurations};
		
		if(idx >= (*configurations).size()) return nullptr;

		AVR::pgm_ptr<const Configuration*> arr{(*configurations).begin()};
		if(arr)
			return arr[idx];
		return nullptr;
	}

	constexpr AVR::pgm_ptr<const std::constexpr_vector<const Interface*>> getInterfaces(const Configuration *dev, uint8_t idx = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr<
			const std::constexpr_vector<
				const std::constexpr_vector<const Interface*>*
				>*
			> p_interfaces{&dev->m_interfaces};
		AVR::pgm_ptr<
			const std::constexpr_vector<
				const std::constexpr_vector<const Interface*>*
				>
			> interfaces{*p_interfaces};
		
		if(idx >= (*interfaces).size()) return AVR::pgm_ptr<const std::constexpr_vector<const Interface*>>{nullptr};

		AVR::pgm_ptr<const std::constexpr_vector<const Interface*>*> arr{(*interfaces).begin()};
		if(arr)
			return AVR::pgm_ptr<const std::constexpr_vector<const Interface*>>{arr[idx]};
		return AVR::pgm_ptr<const std::constexpr_vector<const Interface*>>{nullptr};
	}

	constexpr const Interface* getInterface(const Configuration *dev, uint8_t idx = 0, uint8_t alt = 0)
	{
		//if out of range return nullptr
		AVR::pgm_ptr<
			const std::constexpr_vector<
				const std::constexpr_vector<const Interface*>*
				>*
			> p_interfaces{&dev->m_interfaces};
		AVR::pgm_ptr<
			const std::constexpr_vector<
				const std::constexpr_vector<const Interface*>*
				>
			> interfaces{*p_interfaces};
		
		if(idx >= (*interfaces).size()) return nullptr;

		AVR::pgm_ptr<const std::constexpr_vector<const Interface*>*> arr{(*interfaces).begin()};
		if(!arr) return nullptr;

		AVR::pgm_ptr<const std::constexpr_vector<const Interface*>> elem{arr[idx]};

		std::constexpr_vector<const Interface*> o_interfaces{*elem};
		if(alt >= o_interfaces.size()) return nullptr;

		AVR::pgm_ptr<const Interface*> p_interface{o_interfaces.begin()};
		
		return p_interface[idx];
	}

	constexpr AVR::USB::Endpoint* getEndpoint(const Interface *itf, uint8_t idx = 0)
	{
		//if out of range, return nullptr
		AVR::pgm_ptr<const std::constexpr_vector<Endpoint*>*> p_endpoints{&itf->m_endpoints};
		AVR::pgm_ptr<const std::constexpr_vector<Endpoint*>> endpoints{*p_endpoints};
		
		if(idx >= (*endpoints).size()) return nullptr;

		AVR::pgm_ptr<Endpoint*> arr{(*endpoints).begin()};
		if(arr)
			return arr[idx];
		return nullptr;

	}

	constexpr AVR::USB::StringDescriptorTable::Str getString(const StringDescriptorTable *_tbl, uint8_t idx)
	{
		const StringDescriptorTable tbl = *AVR::pgm_ptr{_tbl};
		return tbl.string(idx);
	}

	constexpr const AVR::USB::StringDescriptorTable* getStringTable(const Device *dev, LanguageID lang = LanguageID::English_United_States)
	{
		AVR::pgm_ptr<const AVR::USB::StringDescriptorTable*> _ptr{&dev->m_strings};
		return *_ptr;
	}
	
} // namespace AVR::USB
