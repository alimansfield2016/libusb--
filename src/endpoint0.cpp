#include <usb/pgm_deref.hpp>
#include <usb/descriptor.hpp>
#include <usb/endpoint0.hpp>

#include "libusb++_ext.hpp"

#include <avr/io/io.hpp>
#include <avr/memory/pgmspace.hpp>

using namespace AVR::USB;

Endpoint0 AVR::USB::_endp0{
	nullptr,
	nullptr
};


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


bool Endpoint0::setup(uint8_t *rxBuf, uint8_t &rxLen)
{
	// if(rxLen < 8) {
	// 	rxLen = 0; 
	// 	return;
	// }

	//We already know the format of the setup packet
	//so we can index it with a struct

	++rxBuf;
	uint8_t bmRequestType = *rxBuf++;
	uint8_t bRequest = *rxBuf++;
	uint16_t wValue = *rxBuf++; wValue |= *rxBuf++ << 8;
	uint16_t wIndex = *rxBuf++; wIndex |= *rxBuf++ << 8;
	uint16_t wCount = *rxBuf++; wCount |= *rxBuf++ << 8;

	//rxBuf[1]
	RequestDirection direction = static_cast<RequestDirection>(bmRequestType>>7);
	RequestRecipient recipient = static_cast<RequestRecipient>(bmRequestType&0x1F);
	RequestType type = static_cast<RequestType>((bmRequestType>>5)&0x03);
	//rxBuf[2]
	Request request = static_cast<Request>(bRequest);

	setDataPID(PID::DATA1);
	if(type != RequestType::Standard)
		return false;
	rxLen = 0;
	switch (request)
	{
	case Request::GetStatus:
		{
			txBuf[0] = 0;
			txBuf[1] = 0;
			genPacket(getDataPID(), 2);
			break;
			switch (recipient)
			{
			case RequestRecipient::Device :
				/* code */
				break;
			
			default:
				break;
			}
		}
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
	return true;
}

bool Endpoint0::out(uint8_t *rxBuf, uint8_t &rxLen, bool _setup)
{
	if(_setup) 
		return setup(rxBuf, rxLen);

	rxLen = 0;
	return false;
}

void Endpoint0::in()
{
	if(txLen()) return;

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
	pageOffset = 0;
	stateIdx = 0;
	switch (type)
	{
	case DescriptorType::Device :
		state = State::DeviceDescriptor;
		buf_ptr = getDeviceDescriptorBuf(pDevice);
		// buf_ptr = AVR::pgm_ptr{*AVR::pgm_ptr{&deviceDescriptor.m_ptr}};
		pageOffset = 0x12;
		loadDeviceDescriptor();
		break;
	case DescriptorType::Configuration :
		state = State::ConfigurationDescriptor;
		p_configuration = getConfiguration(pDevice);
		loadConfigurationDescriptor();
		break;
	case DescriptorType::Interface : break;	//Not directly accessible
	case DescriptorType::Endpoint : break;	//Not directly accessible

	case DescriptorType::String : 
		state = State::StringDescriptor;
		{
			const StringDescriptorTable *tbl = pDevice->getStringTablePgmThisPgm().ptr();
			if(idx){
				StringDescriptorTable::Str str{tbl->stringPgmThis(idx)};
				pageOffset = str.first;
				buf_ptr.assign(reinterpret_cast<const uint8_t*>(str.second));
			}else{
				pageOffset = 2;
				buf_ptr.assign(reinterpret_cast<const uint8_t*>(&tbl->m_language));
			}
		}
		loadStringDescriptor();
		break;
	case DescriptorType::Debug:
		genPacket(getDataPID(), 0);
		break;
	default:
		break;
	}
}

void Endpoint0::loadDeviceDescriptor()
{
	
	uint8_t i;
	for(i = 0; (i < 8) && pageOffset && maxLength; i++, --maxLength, --pageOffset)
		{
			txBuf[i] = *buf_ptr++;
		}
	genPacket(getDataPID(), i);
	if(i < 8) resetState();
	
}

void Endpoint0::loadConfigurationDescriptor()
{
	//first load configuration descriptor
	//then load interface descriptors
	//after each interface descriptor, load relevant endpoint descriptors
	uint8_t i = 0;
	do{
		switch(stateIdx){
			case 0: //init
				buf_ptr = getConfigurationDescriptorBuf(p_configuration);
				// buf_ptr = AVR::pgm_ptr{*AVR::pgm_ptr{&configurationDescriptor.m_ptr}};
				stateIdx++;
				pageOffset = ConfigurationDescriptor::s_size;
				[[fallthrough]];
			case 1:	//configuration descriptor
				for(; i < 8 && pageOffset  && maxLength; i++, maxLength--, pageOffset--)
					txBuf[i] = *buf_ptr++;
				if(pageOffset && i==8)
					break;
				pageOffset = InterfaceDescriptor::s_size;
				stateIdx++;
				intfIdx = 0;
				altIdx = 0;
				nextInterface();
				if(!p_interface){
					state = State::DEFAULT;
					stateIdx = 0;
				}
				buf_ptr = getInterfaceDescriptorBuf(p_interface);
				[[fallthrough]];
			case 2:	//interface descriptor
				for(; i < 8 && pageOffset && maxLength; i++, maxLength--, pageOffset--)
					txBuf[i] = *buf_ptr++;
				if(pageOffset && i==8)
					break;
				pageOffset = EndpointDescriptor::s_size;
				endptIdx = 0;
				nextEndpoint();
				if(!p_endpoint){
					nextInterface();
					if(!p_interface){
						stateIdx = 0;
						state = State::DEFAULT;
					}
					buf_ptr = getInterfaceDescriptorBuf(p_interface);
					pageOffset = InterfaceDescriptor::s_size;
					break;
				}
				buf_ptr = getEndpointDescriptorBuf(p_endpoint);
				stateIdx++;
				[[fallthrough]];
			case 3:	//Endpoint Descriptor
				for(; i < 8 && pageOffset && maxLength; i++, maxLength--, pageOffset--)
					txBuf[i] = *buf_ptr++;
				if(pageOffset && i==8)
					break;
				//Load next Endpoint Descriptor
				nextEndpoint();
				if(p_endpoint){
					buf_ptr = getEndpointDescriptorBuf(p_endpoint);
					pageOffset = EndpointDescriptor::s_size;
					break;
				}
				nextInterface();
				if(p_interface){
					buf_ptr = getInterfaceDescriptorBuf(p_interface);
					pageOffset = InterfaceDescriptor::s_size;
					stateIdx--;
					break;
				}
				[[fallthrough]];
			default:
				state = State::DEFAULT;
				stateIdx = 0;
				break;
		}
	}while(i < 8 && state != State::DEFAULT && maxLength && pageOffset);

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

		txBuf[0] = pageOffset+2;
		txBuf[1] = static_cast<uint8_t>(DescriptorType::String);
		stateIdx++;
		i = 2;
		maxLength-=2;
		[[fallthrough]];	
	case 1:
		for(;i < 8 && pageOffset && maxLength; i++, pageOffset--, maxLength--)
			txBuf[i] = *buf_ptr++;
		if(!pageOffset || !maxLength)
			state = State::DEFAULT;
	default:
		break;
	}
	genPacket(getDataPID(), i);
}

void Endpoint0::nextInterface()
{
	const Interface *p_if;
	p_if = getInterface(p_configuration, intfIdx, altIdx++);
	if(p_if){
		p_interface = p_if;
		return;
	}
	altIdx = 0;
	intfIdx++;
	p_if = getInterface(p_configuration, intfIdx, altIdx++);
	p_interface = p_if;
	if(!p_if){
		intfIdx = 0;
		altIdx = 0;
	}
}
void Endpoint0::nextEndpoint()
{
	const Endpoint *p_ep;
	p_ep = getEndpoint(p_interface, endptIdx++);
	if(!p_ep)
		endptIdx = 0;
	p_endpoint = p_ep;
}
