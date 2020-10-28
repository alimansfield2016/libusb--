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

		}state;
		uint8_t stateIDx;
	public:
		void setup(uint8_t *rxBuf, uint8_t &rxLen) override;
		void out(uint8_t *rxBuf, uint8_t &rxLen) override;
		void in() override;
	private:
	};
} // namespace AVR::USB
