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
		getDescriptor(static_cast<DescriptorType>(wValue>>8), wValue&0xFF);
		break;
	
	default:
		break;
	}
}

void Endpoint0::out(uint8_t *rxBuf, uint8_t &rxLen)
{
	PORTB ^= 0x20;


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
	
	default:
		break;
	}
}

void Endpoint0::setDeviceAddr(uint8_t addr)
{
	PORTB ^= 0x01;
	PORTB ^= 0x01;
	state = State::DEFAULT;
	usbNewDeviceAddr = addr;
	genPacket(getDataPID(), 0);
}

void Endpoint0::getDescriptor(DescriptorType type, uint8_t idx)
{
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

const DeviceDescriptor PROGMEM deviceDescriptor { 
	USB_BCD::USB1_1, 
	DeviceClass::VendorSpecific, 
	0xFF, 
	0x00, 
	0x08, 
	0x16c0, 
	0x05dc, 
	0x0102, 
	0, 0, 0, 1 };

void Endpoint0::loadDeviceDescriptor()
{
	// constexpr uint8_t bLength = 18;
	// constexpr uint8_t bDescriptorType = static_cast<uint8_t>(DescriptorType::Device);
	// constexpr uint16_t bcdUSB = static_cast<uint16_t>(USB_BCD::USB1_1);
	// constexpr uint8_t bDeviceClass = static_cast<uint8_t>(DeviceClass::VendorSpecific);
	// constexpr uint8_t bSubDeviceClass = 0xFF;
	// constexpr uint16_t idVendor = 0x16c0;
	// constexpr uint16_t idProduct = 0x05dc;
	// constexpr uint16_t bcdDevice = 0x0102;



	uint8_t offset = stateIdx++<<3;
	
	AVR::pgm_ptr<uint8_t*> p_ptr{&deviceDescriptor.m_ptr};
	// const uint8_t* const _ptr = *p_ptr;
	AVR::pgm_ptr<uint8_t> ptr{*p_ptr + offset};
	if(offset == 0) maxLength = *ptr;

	uint8_t i;
	for(i = 0; (i < 8) && maxLength; i++, --maxLength)
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

void Endpoint0::loadConfigurationDescriptor()
{
	//first load configuration descriptor
	//then load interface descriptors
	//after each interface descriptor, load relevant endpoint descriptors


	//foreach interface descriptor
		//loadInterfaceDescriptor
		//foreach endpoint descriptor
			//loadEndpointDescriptor

	

	
}