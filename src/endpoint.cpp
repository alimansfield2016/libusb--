#include <usb/endpoint.hpp>
#include <avr/interrupt/interrupt.hpp>
// #include "usbdrv.h"
#include "libusb++_ext.hpp"


using namespace AVR::USB;

// AVR::pgm_ptr<uint8_t> Endpoint::DescriptorBuf() const 
// {
// 	return m_descriptor->ptr_pgm();
// }

void EndpointIn::genPacket(PID pid, uint8_t dataLen)
{
	txLenBuf[1] = static_cast<uint8_t>(pid);
	usbCrc16Append(txLenBuf + 2, dataLen);
	{
		AVR::Interrupt::InterruptHolder hold;
		txLenBuf[0] = dataLen+3;
	}
}

EndpointDirection Endpoint::direction() const
{
	AVR::pgm_ptr desc = DescriptorBuf();
	uint8_t v = desc[2];
	return static_cast<EndpointDirection>((v&0x80)?true:false);
}

uint8_t Endpoint::endpointNo() const
{
	AVR::pgm_ptr desc = DescriptorBuf();
	uint8_t v = desc[2];
	return v&0x0F;
}