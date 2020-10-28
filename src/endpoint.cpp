#include <endpoint.hpp>
#include "usbdrv.h"

using namespace AVR;

USB::Endpoint *USB::endp0{nullptr};

void USB::Endpoint::genPacket(USB::PID pid, uint8_t dataLen)
{
	txLenBuf[1] = static_cast<uint8_t>(pid);
	usbCrc16Append(txLenBuf + 2, dataLen);
	txLenBuf[0] = dataLen+3;
}

void USB::Endpoint::setup(uint8_t *rxBuf, uint8_t &rxLen)
{
	
}

void USB::Endpoint::out(uint8_t *rxBuf, uint8_t &rxLen)
{
	
}

void USB::Endpoint::in()
{
	
}