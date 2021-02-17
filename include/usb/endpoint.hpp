#pragma once

#include <usb/libusb++.hpp>
#include <usb/descriptor.hpp>
#include <stdint.h>
#include <avr/memory/pgmspace.hpp>

namespace AVR::USB
{
	class Endpoint;


	class EndpointIn;
	class EndpointOut;
	class EndpointInOut;

	class EndpointDescriptor;

	class Endpoint
	{
		//PROGMEM ptr
		AVR::pgm_ptr<EndpointDescriptor> m_descriptor;
	public:
		Endpoint(AVR::pgm_ptr<EndpointDescriptor> _descriptor) : m_descriptor{_descriptor} {}
		[[nodiscard]]::AVR::pgm_ptr<uint8_t> DescriptorBuf() const
		{
			return m_descriptor->ptr_pgm();
		}
		EndpointDirection direction() const;
		// {
		// 	AVR::pgm_ptr desc = DescriptorBuf();
		// 	uint8_t v = desc[2];
		// 	return static_cast<EndpointDirection>((v&0x80)?true:false);
		// }
		uint8_t endpointNo() const;
	};

	class EndpointIn : public Endpoint
	{
		friend class Endpoint0;
		uint8_t txLenBuf[12];
		PID DataPID;
	protected:
		uint8_t *txBuf() { return txLenBuf+2; }
		void genPacket(PID pid, uint8_t dataLen);
		PID getDataPID() { PID pid = DataPID; if(DataPID == PID::DATA0) DataPID = PID::DATA1; else DataPID = PID::DATA0; return pid; }
		void setDataPID(PID pid) { DataPID = pid; }
		void clear() { txLenBuf[0] = 0; }
	public:
		EndpointIn(AVR::pgm_ptr<EndpointDescriptor> _descriptor) : 
			Endpoint{_descriptor},
			DataPID{PID::DATA0}
				{}
		virtual void in() = 0;
		uint8_t *buf() { return txLenBuf; }
		uint8_t txLen() const { return txLenBuf[0]; }
	};
	class EndpointOut : public Endpoint
	{
	public:
		EndpointOut(AVR::pgm_ptr<EndpointDescriptor> _descriptor) : Endpoint{_descriptor} {}
		virtual bool out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) = 0;
	};

	class EndpointInOut : public EndpointIn, public EndpointOut
	{
	public:
		EndpointInOut(AVR::pgm_ptr<EndpointDescriptor> _descriptorIn, AVR::pgm_ptr<EndpointDescriptor> _descriptorOut) : 
			EndpointIn{_descriptorIn},
			EndpointOut{_descriptorOut} {}
	};



} // namespace AVR::USB
