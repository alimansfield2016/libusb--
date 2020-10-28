#pragma once

#include <libusb++.hpp>
#include <stdint.h>

namespace AVR::USB
{
	class Endpoint;
	extern Endpoint *endp0;

	enum class RequestDirection : bool{
		HostToDevice = false,
		DeviceToHost = true,
	};

	enum class RequestType : uint8_t{
		Standard = 0,
		Class = 1,
		Vendor = 2,
		RESERVED = 3,
	};

	enum class RequestRecipient : uint8_t{
		Device = 0,
		Interface = 1,
		Endpoint = 2,
		Other = 3,
	};

	enum class DescriptorType : uint8_t{
		Device = 1,
		Configuration = 2,
		String = 3,
		Interface = 4,
		Endpoint = 5,
		DeviceQualifier = 6,
		OtherSpeedConfiguration = 7,
		InterfacePower = 8,
		OnTheGo = 9,
	};

	class Endpoint
	{
		uint8_t txLenBuf[12];
	protected:
		uint8_t *txBuf;
		void genPacket(PID pid, uint8_t dataLen);
	public:
		uint8_t *buf() { return txLenBuf; }
		virtual void setup(uint8_t *rxBuf, uint8_t &rxLen);
		virtual void out(uint8_t *rxBuf, uint8_t &rxLen);
		virtual void in();
	};
} // namespace AVR::USB
