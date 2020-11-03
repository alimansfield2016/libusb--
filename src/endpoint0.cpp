#include <pgm_deref.hpp>

#include "endpoint0.hpp"
#include "descriptor.hpp"
#include "libusb++_ext.hpp"

#include <avr/io/io.hpp>
#include <avr/memory/pgmspace.hpp>

using namespace AVR::USB;

Endpoint0 AVR::USB::_endp0{};

void Endpoint0::setup(uint8_t *rxBuf, uint8_t &rxLen)
{
	// if(rxLen < 8) {
	// 	rxLen = 0; 
	// 	return;
	// }
	PORTB ^= 0x40;
	PORTB ^= 0x40;


	//We already know the format of the setup packet
	//so we can index it with a struct

	++rxBuf;
	uint8_t bmRequestType = *rxBuf++;
	uint8_t bRequest = *rxBuf++;
	uint16_t wValue = *rxBuf++; wValue |= *rxBuf++ << 8;
	uint16_t wIndex = *rxBuf++; wIndex |= *rxBuf++ << 8;
	uint16_t wCount = *rxBuf++; wCount |= *rxBuf++ << 8;
	rxLen = 0;

	//rxBuf[1]
	RequestDirection direction = static_cast<RequestDirection>(bmRequestType>>7);
	RequestRecipient recipient = static_cast<RequestRecipient>(bmRequestType&0x1F);
	RequestType type = static_cast<RequestType>((bmRequestType>>5)&0x03);
	//rxBuf[2]
	Request request = static_cast<Request>(bRequest);

	setDataPID(PID::DATA1);
	switch (request)
	{
	case Request::SetAddress:
		setDeviceAddr(wValue&0x7F);
		break;
	case Request::GetDescriptor:
		maxLength = wCount;
		getDescriptor(static_cast<DescriptorType>(wValue>>8), wValue&0xFF);
		break;
	case Request::SetConfiguration:
		setConfiguration(wValue);
		break;
	
	default:
		break;
	}
}

void Endpoint0::out(uint8_t *rxBuf, uint8_t &rxLen, bool _setup)
{
	PORTB ^= 0x20;

	if(_setup) {
		setup(rxBuf, rxLen);
		return;
	}

	rxLen = 0;
}

void Endpoint0::in()
{
	if(txLen()) return;

	PORTB ^= 0x10;
	PORTB ^= 0x10;

	switch (state)
	{
	case State::DeviceDescriptor :
		loadDeviceDescriptor();
		break;
	case State::ConfigurationDescriptor :
		loadConfigurationDescriptor();
		break;
	case State::StringDescriptor :
		loadStringDescriptor();
		break;
	
	default:
		break;
	}
}

void Endpoint0::setDeviceAddr(uint8_t addr)
{
	PORTB ^= 0x01;
	PORTB ^= 0x01;
	resetState();
	usbNewDeviceAddr = addr;
	genPacket(getDataPID(), 0);
}

void Endpoint0::setConfiguration(uint16_t config)
{
	resetState();
	genPacket(getDataPID(), 0);
}

void Endpoint0::getDescriptor(DescriptorType type, uint8_t idx)
{
	offset = 0;
	stateIdx = 0;
	switch (type)
	{
	case DescriptorType::Device :
		state = State::DeviceDescriptor;
		buf_ptr = getDeviceDescriptorBuf(pDevice);
		loadDeviceDescriptor();
		break;
	case DescriptorType::Configuration :
		state = State::ConfigurationDescriptor;
		loadConfigurationDescriptor();
		break;
	case DescriptorType::Interface : break;
	case DescriptorType::Endpoint : break;

	case DescriptorType::String : 
		state = State::StringDescriptor;
		{
			const StringDescriptorTable *tbl = pDevice->getStringTablePgmThisPgm().ptr();
			if(idx){
				StringDescriptorTable::Str str{tbl->stringPgmThis(idx)};
				offset = str.first;
				buf_ptr.assign(reinterpret_cast<const uint8_t*>(str.second));
			}else{
				offset = 2;
				buf_ptr.assign(reinterpret_cast<const uint8_t*>(&tbl->m_language));
			}
		}
		loadStringDescriptor();
		break;
	
	default:
		break;
	}
}

// constexpr AVR::USB::DeviceDescriptor PROGMEM deviceDescriptor { 
// 	USB_BCD::USB1_1, 
// 	DeviceClass::VendorSpecific, 
// 	0xFF, 
// 	0x00, 
// 	0x08, 
// 	0x16c0, 
// 	0x05dc, 
// 	0x0103, 
// 	0, 0, 0, 1
// };

void Endpoint0::loadDeviceDescriptor()
{
	
	uint8_t i;
	for(i = 0; (i < 8) && (offset < 0x12) && maxLength; i++, --maxLength, offset++)
		{
			txBuf[i] = *buf_ptr++;
		}
	genPacket(getDataPID(), i);
	if(i < 8) resetState();
	
}

// constexpr AVR::USB::ConfigurationDescriptor PROGMEM configurationDescriptor {
// 	0x12,
// 	1,
// 	1,
// 	0,
// 	ConfigurationAttributes::NONE,
// 	100_mA,
// };
// constexpr AVR::USB::InterfaceDescriptor PROGMEM interface0Descriptor {
// 	0,
// 	0,
// 	0,
// 	InterfaceClass::VendorSpecific,
// 	0,
// 	0,
// 	0,
// };


void Endpoint0::loadConfigurationDescriptor()
{
	//first load configuration descriptor
	//then load interface descriptors
	//after each interface descriptor, load relevant endpoint descriptors
	uint8_t i = 0;

	switch(stateIdx){
		case 0: //init
			buf_ptr = getConfigurationDescriptorBuf(getConfiguration(pDevice));
			stateIdx++;
			offset = 9;
			[[fallthrough]];
		case 1:	//configuration descriptor
			for(; i < 8 && offset  && maxLength; i++, maxLength--, offset--)
				txBuf[i] = *buf_ptr++;
			if(i==8)
				break;
			offset = 9;
			stateIdx++;
			buf_ptr = getInterfaceDescriptorBuf(getInterface(getConfiguration(pDevice)));
			[[fallthrough]];
		case 2:	//interface descriptor
			for(; i < 8 && offset && maxLength; i++, maxLength--, offset--)
				txBuf[i] = *buf_ptr++;
			if(i==8)
				break;
			offset = 0;
			state = State::DEFAULT;
			stateIdx = 0;
			break;
	}

	genPacket(getDataPID(), i);


	//foreach interface descriptor
		//loadInterfaceDescriptor
		//foreach endpoint descriptor
			//loadEndpointDescriptor

}

void Endpoint0::loadStringDescriptor()
{
	uint8_t i = 0;
	switch (stateIdx)
	{
	case 0: //init

		txBuf[0] = offset+2;
		txBuf[1] = static_cast<uint8_t>(DescriptorType::String);
		stateIdx++;
		i = 2;
		maxLength-=2;
		[[fallthrough]];	
	case 1:
		for(;i < 8 && offset && maxLength; i++, offset--, maxLength--)
			txBuf[i] = *buf_ptr++;
		if(!offset || !maxLength)
			state = State::DEFAULT;
	default:
		break;
	}
	genPacket(getDataPID(), i);
}