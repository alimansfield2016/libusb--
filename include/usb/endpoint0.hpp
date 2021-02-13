#pragma once

#include <usb/endpoint.hpp>

#include <usb/device.hpp>

#include <avr/memory/pgmspace.hpp>

namespace AVR::USB
{
	class Endpoint0;
	extern Endpoint0 _endp0;
	class Interface;

	class Endpoint0 : public EndpointInOut
	{
		//state to keep track of transmissions
		enum class State : uint8_t{
			DEFAULT,
			DeviceDescriptor,
			ConfigurationDescriptor,
			StringDescriptor,
		}state;
		uint8_t stateIdx;
		uint8_t pageOffset;
		uint16_t maxLength;
		uint8_t intfIdx;
		uint8_t altIdx;
		uint8_t endptIdx;
		AVR::pgm_ptr<uint8_t> buf_ptr;
		//Configuration storage class in PROGMEM
		AVR::pgm_ptr<Configuration> p_configuration;
		//Interface storage class in PROGMEM
		AVR::pgm_ptr<Interface> p_interface;
		//Endpoint is an active class stored in SRAM
		Endpoint *p_endpoint;
		const StringDescriptorTable *p_stringTbl;
		RequestType m_reqType;
		bool m_configurationSet;
	public:
		Endpoint0(
			const EndpointDescriptor *_descIn,
			const EndpointDescriptor *_descOut
		) : 
			EndpointInOut{_descIn, _descOut},
			buf_ptr{nullptr},
			p_configuration{nullptr},
			p_interface{nullptr},
			m_configurationSet{false}
		 {}
		bool out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) override;
		bool setup(uint8_t *rxBuf, uint8_t &rxLen);
		void in() override;
		RequestType reqType() const { return m_reqType; }
		bool configurationSet() const volatile { return m_configurationSet; }
	private:
		void setDeviceAddr(uint8_t addr);
		void setConfiguration(uint8_t config);
		void getDescriptor(DescriptorType type, uint8_t idx);

		void loadDeviceDescriptor();
		void loadConfigurationDescriptor();
		void loadStringDescriptor();

		void loadDebugDescriptor();

		void nextInterface();
		void nextEndpoint();

		void resetState() { state = State::DEFAULT; stateIdx = 0; }
	};
} // namespace AVR::USB
