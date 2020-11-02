#pragma once

#include <libusb++.hpp>
#include <stdint.h>
#include <avr/memory/pgmspace.hpp>
#include <descriptor.hpp>

namespace AVR::USB
{
	class Endpoint;


	class EndpointIn;
	class EndpointOut;
	class EndpointInOut;

	class EndpointDescriptor;

	class Endpoint
	{
	public:
		//PROGMEM ptr
		const EndpointDescriptor * m_descriptor;
		 [[nodiscard]]::AVR::pgm_ptr<const uint8_t> DescriptorBuf() const;
	};

	class EndpointIn : public Endpoint
	{
		uint8_t txLenBuf[12];
		PID DataPID;
	protected:
		uint8_t *txBuf;
		void genPacket(PID pid, uint8_t dataLen);
		PID getDataPID() { PID pid = DataPID; if(DataPID == PID::DATA0) DataPID = PID::DATA1; else DataPID = PID::DATA0; return pid; }
		void setDataPID(PID pid) { DataPID = pid; }
	public:
		EndpointIn();
		virtual void in() = 0;
		uint8_t *buf() { return txLenBuf; }
		uint8_t txLen() const { return txLenBuf[0]; }
	};
	class EndpointOut : public Endpoint
	{
	public:
		virtual void out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) = 0;
	};

	class EndpointSetup : public EndpointOut
	{
	public:
		virtual void setup() = 0;
	};

	class EndpointInOut : public EndpointIn, public EndpointOut
	{
	public:
	};

	class _Endpoint
	{
		uint8_t txLenBuf[12];
		PID DataPID;
	protected:
		uint8_t *txBuf;
		void genPacket(PID pid, uint8_t dataLen);
		PID getDataPID() { PID pid = DataPID; if(DataPID == PID::DATA0) DataPID = PID::DATA1; else DataPID = PID::DATA0; return pid; }
		void setDataPID(PID pid) { DataPID = pid; }
	public:
		_Endpoint();
		uint8_t *buf() { return txLenBuf; }
		uint8_t txLen() const { return txLenBuf[0]; }
		virtual void setup(uint8_t *rxBuf, uint8_t &rxLen);
		virtual void out(uint8_t *rxBuf, uint8_t &rxLen);
		virtual void in();
	};
} // namespace AVR::USB
