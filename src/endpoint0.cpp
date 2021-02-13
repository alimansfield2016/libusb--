#include <usb/pgm_deref.hpp>
#include <usb/descriptor.hpp>
#include <usb/endpoint0.hpp>

#include "libusb++_ext.hpp"

#include <avr/io/io.hpp>
#include <avr/io/serial.hpp>
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
	//We already know the format of the setup packet
	//so we can index it with a struct

	++rxBuf;
	uint8_t bmRequestType = *rxBuf++;
	uint8_t bRequest = *rxBuf++;
	uint16_t wValue = *rxBuf++; wValue |= *rxBuf++ << 8;
	uint16_t wIndex = *rxBuf++; wIndex |= *rxBuf++ << 8;
	uint16_t wCount = *rxBuf++; wCount |= *rxBuf++ << 8;

	//rxBuf[1]
	// RequestDirection direction = static_cast<RequestDirection>(bmRequestType>>7);
	RequestRecipient recipient = static_cast<RequestRecipient>(bmRequestType&0x1F);
	m_reqType = static_cast<RequestType>((bmRequestType>>5)&0x03);
	//rxBuf[2]
	Request request = static_cast<Request>(bRequest);

	setDataPID(PID::DATA1);
	if(m_reqType != RequestType::Standard)
		return false;
	rxLen = 0;
	switch (request)
	{
	case Request::GetStatus:
		{
			switch (recipient)
			{
			case RequestRecipient::Device :
				txBuf[0] = 0;
				txBuf[1] = 0;
				genPacket(getDataPID(), 2);
				break;
			
			default:
				break;
			}
		}
		break;
	case Request::SetAddress:
		setDeviceAddr(wValue&0x7F);
		break;
	case Request::GetDescriptor:
		maxLength = wCount;
		getDescriptor(static_cast<DescriptorType>(wValue>>8), wValue&0xFF);
		break;
	case Request::SetConfiguration:
		setConfiguration(wValue&0xFF);
		break;
	
	default:
		break;
	}
	return true;
}

bool Endpoint0::out(uint8_t *rxBuf, uint8_t &rxLen, bool _setup)
{
	if(_setup) 
		return Endpoint0::setup(rxBuf, rxLen);

	// rxLen = 0;
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

void Endpoint0::setConfiguration(uint8_t config)
{
	//Configuration index starts at 1!
	--config;
	USART0.Print('C');
	// resetState();

	//setup relevant endpoints.
	p_configuration = getConfiguration(pDevice, config);
	intfIdx = 0;
	while((p_interface = getInterface(p_configuration, intfIdx++)))
	{
		endptIdx = 0;
		while((p_endpoint = getEndpoint(p_interface, endptIdx++)))
		{
			uint8_t endptNo = p_endpoint->endpointNo();
			if(!endptNo) break;
			if(p_endpoint->direction() == EndpointDirection::IN){
				EndpointsIn[endptNo] = static_cast<EndpointIn*>(p_endpoint);
				usbTxLenBufs[endptNo] = static_cast<EndpointIn*>(p_endpoint)->txLenBuf;
			}else{
				EndpointsOut[endptNo] = static_cast<EndpointOut*>(p_endpoint);
			}
		}
	}
	USART0.Print('c');
	m_configurationSet = true;
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
		pageOffset = DeviceDescriptor::s_size;
		loadDeviceDescriptor();
		break;
	case DescriptorType::Configuration :
		state = State::ConfigurationDescriptor;
		p_configuration = getConfiguration(pDevice);
		buf_ptr = getConfigurationDescriptorBuf(p_configuration);
		pageOffset = ConfigurationDescriptor::s_size;
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
		for(; i < 8 && pageOffset  && maxLength; i++, maxLength--, pageOffset--)
			txBuf[i] = *buf_ptr++;
		if(!pageOffset || i!=8){
			switch(stateIdx){
				case 0:	//configuration descriptor
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
					break;
				case 1:	//interface descriptor
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
					break;;
				case 2:	//Endpoint Descriptor
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
				default: //If all else fails
					state = State::DEFAULT;
					stateIdx = 0;
					break;
			}
		}
	}while(i < 8 && state != State::DEFAULT && maxLength && pageOffset);

	genPacket(getDataPID(), i);
	if(i < 8) resetState();
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
		// if(!pageOffset || !maxLength)
		if(i == 8)
			break;
		[[fallthrough]];
	default:
		state = State::DEFAULT;
		break;
	}
	genPacket(getDataPID(), i);
}

void Endpoint0::nextInterface()
{
	p_interface = getInterface(p_configuration, intfIdx, altIdx++);
	if(p_interface) return;
	altIdx = 0;
	intfIdx++;
	p_interface = getInterface(p_configuration, intfIdx, altIdx++);
	if(!p_interface){
		intfIdx = 0;
		altIdx = 0;
	}
}
void Endpoint0::nextEndpoint()
{
	Endpoint *p_ep;
	p_ep = getEndpoint(p_interface, endptIdx++);
	if(!p_ep)
		endptIdx = 0;
	p_endpoint = p_ep;
}
