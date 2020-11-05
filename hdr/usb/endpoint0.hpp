#pragma once

#include <usb/endpoint.hpp>

#include <usb/device.hpp>

namespace AVR::USB
{
	class Endpoint0;
	extern Endpoint0 _endp0;

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
		const Configuration *p_configuration;
		const Interface *p_interface;
		const Endpoint *p_endpoint;
		const StringDescriptorTable *p_stringTbl;
	public:
		Endpoint0(
			const EndpointDescriptor *_descIn,
			const EndpointDescriptor *_descOut
		) : EndpointInOut{_descIn, _descOut} {}
		bool out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) override;
		bool setup(uint8_t *rxBuf, uint8_t &rxLen);
		void in() override;
	private:
		void setDeviceAddr(uint8_t addr);
		void setConfiguration(uint16_t config);
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
