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
	switch (type)
	{
	case DescriptorType::Device :
		stateIdx = 0;
		state = State::DeviceDescriptor;
		loadDeviceDescriptor();
		break;
	case DescriptorType::Configuration :
		stateIdx = 0;
		state = State::ConfigurationDescriptor;
		loadConfigurationDescriptor();
	case DescriptorType::Interface :
	case DescriptorType::Endpoint :
	case DescriptorType::String :
		/* code */
		break;
	
	default:
		break;
	}
}

constexpr AVR::USB::DeviceDescriptor PROGMEM deviceDescriptor { 
	USB_BCD::USB1_1, 
	DeviceClass::VendorSpecific, 
	0xFF, 
	0x00, 
	0x08, 
	0x16c0, 
	0x05dc, 
	0x0103, 
	0, 0, 0, 1
};

void Endpoint0::loadDeviceDescriptor()
{
	
	AVR::pgm_ptr<const uint8_t*> p_ptr{&deviceDescriptor.m_ptr};
	// const uint8_t* const _ptr = *p_ptr;
	AVR::pgm_ptr<uint8_t> ptr{*p_ptr + offset};

	uint8_t i;
	for(i = 0; (i < 8) && (offset < 0x12) && maxLength; i++, --maxLength, offset++)
		{
			txBuf[i] = *ptr++;
		}
	genPacket(getDataPID(), i);
	if(i < 8) resetState();
	
	/*
	switch (stateIdx++)
	{
	case 0:
		{
			//bLength
			txBuf[0] = bLength;
			//bDescriptorType
			txBuf[1] = bDescriptorType;
			//bcdUSB
			txBuf[2] = bcdUSB&0xFF;
			txBuf[3] = bcdUSB>>8;
			//bDeviceClass
			txBuf[4] = 0xFF;
			//bDeviceSubClass
			txBuf[5] = 0x00;
			//bDeviceProtocol
			txBuf[6] = 0x00;
			//bMaxPacketSize0
			txBuf[7] = 0x08;
			genPacket(getDataPID(), 8);
		}
		break;
	case 1:
		{
			//idVendor
			txBuf[0] = idVendor&0xFF;
			txBuf[1] = idVendor>>8;
			//idProduct
			txBuf[2] = idProduct&0xFF;
			txBuf[3] = idProduct>>8;
			//bcdDevice
			txBuf[4] = bcdDevice&0xFF;
			txBuf[5] = bcdDevice>>8;
			//iManufacturer
			txBuf[6] = 0;
			//iProduct
			txBuf[7] = 0;
			genPacket(getDataPID(), 8);
		}
		break;
	case 2:
		{
			//iSerialNumber
			txBuf[0] = 0;
			//bNumConfigurations
			txBuf[1] = 1;
			genPacket(getDataPID(), 2);
			resetState();
		}
		break;
	
	default:
		stateIdx--;
		break;
	}
	*/
}

constexpr AVR::USB::ConfigurationDescriptor PROGMEM configurationDescriptor {
	0x12,
	1,
	1,
	0,
	ConfigurationAttributes::NONE,
	100_mA,
};

constexpr AVR::USB::InterfaceDescriptor PROGMEM interface0Descriptor {
	0,
	0,
	0,
	InterfaceClass::VendorSpecific,
	0,
	0,
	0,
};


void Endpoint0::loadConfigurationDescriptor()
{
	//first load configuration descriptor
	//then load interface descriptors
	//after each interface descriptor, load relevant endpoint descriptors
	constexpr AVR::pgm_ptr<const uint8_t*> cd_ptr{&configurationDescriptor.m_ptr};
	constexpr AVR::pgm_ptr<const uint8_t*> id_ptr{&interface0Descriptor.m_ptr};
	AVR::pgm_ptr<uint8_t> ptr;
	uint8_t i = 0;

	switch(stateIdx){
		case 0:	//configuration descriptor
			ptr.assign(*cd_ptr + offset);
			for(; i < 8 && offset < 9 && maxLength; i++, ptr++, maxLength--, offset++)
				txBuf[i] = *ptr;
			if(i==8)
				break;
			offset = 0;
			stateIdx++;
			[[fallthrough]];
		case 1:	//interface descriptor
			ptr.assign(*id_ptr + offset);
			for(; i < 8 && offset < 9 && maxLength; i++, ptr++, maxLength--, offset++)
				txBuf[i] = *ptr;
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