#include <usb/endpoint.hpp>
#include "usbdrv.h"

using namespace AVR::USB;

AVR::pgm_ptr<const uint8_t> Endpoint::DescriptorBuf() const 
{
	return AVR::pgm_ptr<const uint8_t>(
		*AVR::pgm_ptr<const uint8_t*>(&m_descriptor->m_ptr)
		);
}

EndpointIn::EndpointIn() : 
	DataPID{PID::DATA0},
	txBuf{&txLenBuf[2]}
{}

void EndpointIn::genPacket(PID pid, uint8_t dataLen)
{
	txLenBuf[1] = static_cast<uint8_t>(pid);
	usbCrc16Append(txLenBuf + 2, dataLen);
	txLenBuf[0] = dataLen+3;
}
