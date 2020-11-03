#pragma once

#include <endpoint.hpp>

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
		uint8_t offset;
		uint16_t maxLength;
		AVR::pgm_ptr<uint8_t> buf_ptr;
	public:
		void out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) override;
		bool setup(uint8_t *rxBuf, uint8_t &rxLen);
		void in() override;
	private:
		void setDeviceAddr(uint8_t addr);
		void setConfiguration(uint16_t config);
		void getDescriptor(DescriptorType type, uint8_t idx);

		void loadDeviceDescriptor();
		void loadConfigurationDescriptor();
		void loadStringDescriptor();

		void resetState() { state = State::DEFAULT; stateIdx = 0; }
	};
} // namespace AVR::USB
