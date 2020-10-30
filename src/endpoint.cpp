#include <endpoint.hpp>
#include "usbdrv.h"

using namespace AVR::USB;

Endpoint::Endpoint() : 
	DataPID{PID::DATA0},
	txBuf{&txLenBuf[2]}
{}

void Endpoint::genPacket(PID pid, uint8_t dataLen)
{
	txLenBuf[1] = static_cast<uint8_t>(pid);
	usbCrc16Append(txLenBuf + 2, dataLen);
	txLenBuf[0] = dataLen+3;
}

void Endpoint::setup(uint8_t *rxBuf, uint8_t &rxLen)
{
	
}

void Endpoint::out(uint8_t *rxBuf, uint8_t &rxLen)
{
	
}

void Endpoint::in()
{
	
}