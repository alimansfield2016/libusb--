#pragma once

#include <endpoint.hpp>

namespace AVR::USB
{
	class Endpoint0;
	extern Endpoint0 _endp0;

	class Endpoint0 : public Endpoint
	{
		//state to keep track of transmissions
		enum class State : uint8_t{
			DEFAULT,
			DeviceDescriptor,
			ConfigurationDescriptor,
		}state;
		uint8_t stateIdx;
		uint8_t offset;
		uint16_t maxLength;
	public:
		void setup(uint8_t *rxBuf, uint8_t &rxLen) override;
		void out(uint8_t *rxBuf, uint8_t &rxLen) override;
		void in() override;
	private:
		void setDeviceAddr(uint8_t addr);
		void setConfiguration(uint16_t config);
		void getDescriptor(DescriptorType type, uint8_t idx);

		void loadDeviceDescriptor();
		void loadConfigurationDescriptor();

		void resetState() { state = State::DEFAULT; stateIdx = 0; }
	};
} // namespace AVR::USB
