#include <usb/pgm_deref.hpp>
#include <usb/descriptor.hpp>
#include <usb/endpoint0.hpp>

#include "libusb++_ext.hpp"

#include <avr/memory/pgmspace.hpp>

// #define DEBUG
#ifdef DEBUG
#include <avr/io/serial.hpp>
#define DEBUG_CHAR(c) USART0.putc(c)
#else
#define DEBUG_CHAR(c)
#endif
#define DEBUG_WORD(w) DEBUG_CHAR((w>>8)); DEBUG_CHAR((w))

using namespace AVR::USB;

Endpoint0 AVR::USB::_endp0{
	AVR::pgm_ptr<AVR::USB::EndpointDescriptor>{nullptr},
	AVR::pgm_ptr<AVR::USB::EndpointDescriptor>{nullptr}
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
	//we have all the data we need right now!
	//and we own this transaction
	rxLen = 0;
	switch (request)
	{
	case Request::GetStatus:
		{
			switch (recipient)
			{
			case RequestRecipient::Device :
				txBuf()[0] = 0;
				txBuf()[1] = 0;
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
	usbNewDeviceAddr = addr;
	// usbDeviceAddr = addr;
	resetState();
	genPacket(getDataPID(), 0);
}

void Endpoint0::setConfiguration(uint8_t config)
{
	//Configuration index starts at 1!
	--config;
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
			//disallow overwriting endp0
			//4 bytes/2 words (Os)
			//6 bytes/3 words (O3)

			#if !(ALLOW_EPT0_SET)
			if(!endptNo) break;
			#endif

			if(p_endpoint->direction() == EndpointDirection::IN){
				EndpointsIn[endptNo] = static_cast<EndpointIn*>(p_endpoint);
				usbTxLenBufs[endptNo] = static_cast<EndpointIn*>(p_endpoint)->buf();
			}else{
				EndpointsOut[endptNo] = static_cast<EndpointOut*>(p_endpoint);
			}
		}
	}
	m_configurationSet = true;
	genPacket(getDataPID(), 0);
}

/**
 * Disable tail-call optimisation since there are 
 * many call-return statements in this function. 
 * This prevents multiple exit-points
 */

#pragma GCC push_options
#pragma GCC optimize ("no-optimize-sibling-calls")

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
		p_configuration = getConfiguration(pDevice, idx);
		// assume that this is ALWAYS valid
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

#pragma GCC pop_options

void Endpoint0::loadDeviceDescriptor()
{
	
	uint8_t i;
	for(i = 0; (i < 8) && pageOffset && maxLength; i++, --maxLength, --pageOffset)
	{
		txBuf()[i] = *buf_ptr++;
	}
	if(i < 8) resetState();
	genPacket(getDataPID(), i);
}

void Endpoint0::loadConfigurationDescriptor()
{
	//first load configuration descriptor
	//then load interface descriptors
	//after each interface descriptor, load relevant endpoint descriptors
	uint8_t i = 0;
	do{
		for(; i < 8 && pageOffset  && maxLength; i++, maxLength--, pageOffset--)
			txBuf()[i] = *buf_ptr++;
		if(!pageOffset || i!=8){
			switch(stateIdx){
				case 0:	//loaded configuration descriptor
					//initial setup state
					stateIdx = 1;
					intfIdx = 0;
					altIdx = 0;
					[[fallthrough]];
				case 1:	//load next interface descriptor
					pageOffset = InterfaceDescriptor::s_size;
					stateIdx=2;	// 2
					nextInterface();
					if(!p_interface){
						//if we run out of interfaces
						//we're done!
						state = State::DEFAULT;
						break;
					}
					//reset endpoint idx
					endptIdx = 0;
					buf_ptr = getInterfaceDescriptorBuf(p_interface);
					break;
				case 2:	//loaded endpoint/interface descriptor
						//load next endpoint descriptor
					nextEndpoint();
					if(!p_endpoint){
						//get the next interface
						stateIdx = 1;
					} else {
						pageOffset = EndpointDescriptor::s_size;
						buf_ptr = p_endpoint->DescriptorBuf();
					}
					break;
				default: //If all else fails
					state = State::DEFAULT;
					break;
			}
		}
	}while(i < 8 && state != State::DEFAULT && maxLength && pageOffset);

	if(i < 8) resetState();
	genPacket(getDataPID(), i);
}

void Endpoint0::loadStringDescriptor()
{
	uint8_t i = 0;
	switch (stateIdx)
	{
	case 0: //init
		//descriptor length (strlen+2)
		//descriptor type (string)
		txBuf()[0] = pageOffset+2;
		txBuf()[1] = static_cast<uint8_t>(DescriptorType::String);
		stateIdx=1;
		i = 2;
		maxLength-=2;
		[[fallthrough]];	
	case 1:
		for(;i < 8 && pageOffset && maxLength; i++, pageOffset--, maxLength--)
			txBuf()[i] = *buf_ptr++;
		if(i == 8)	//continue next time
			break;
		//otherwise, reset
		[[fallthrough]];
	default:
		resetState();
		break;
	}
	genPacket(getDataPID(), i);
}

void Endpoint0::nextInterface()
{
	p_interface = getInterface(p_configuration, intfIdx, altIdx);
	if(!p_interface) {
		altIdx = 0;
		intfIdx++;
		p_interface = getInterface(p_configuration, intfIdx, altIdx);
		//we're starting to cut corners now! (not resetting intfIdx=0 if NULL)
		//but technically we should be initialising 
		//this stuff ourselves before use
	}
	//cutting corners here too (only inc if not NULL)
	altIdx++;
}
void Endpoint0::nextEndpoint()
{
	p_endpoint = getEndpoint(p_interface, endptIdx++);
	//cutting corners again (not resetting endptIdx if NULL)
	//but since this is my code, I can justify
	//removing all the redundancy
}
