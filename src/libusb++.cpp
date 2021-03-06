#include <usb/libusb++.hpp>
#include <usb/device.hpp>
#include <usb/configuration.hpp>
#include <usb/interface.hpp>
#include <usb/endpoint.hpp>
#include <usb/endpoint0.hpp>

#include "libusb++_ext.hpp"
#include "../usbConfig.hpp"

#include <avr/interrupt/interrupt.hpp>
#include <avr/interrupt/watchdog.hpp>

#include <util/delay.h>

#include <array>

using namespace AVR;

uint8_t usbDeviceAddr;
uint8_t usbNewDeviceAddr;
uint8_t usbRxBuf[USB_BUF_LEN*2];
uint8_t usbRxLen;
uint8_t usbRxToken;
uint8_t usbCurrentTok;
uint8_t usbInputBufOffset;
volatile uint8_t usbEndptNoBegin;
volatile uint8_t usbEndptNoData;

uint8_t *usbTxLenBufs[MAX_ENDPTS];
bool usbTransactionEnd;

std::array<USB::EndpointOut*, MAX_ENDPTS> EndpointsOut;
std::array<USB::EndpointIn*, MAX_ENDPTS> EndpointsIn;

AVR::pgm_ptr<AVR::USB::Device> AVR::USB::pDevice{nullptr};

void USB::disconnect()
{
	using namespace USB;
	//set low
	PORT &= ~(DMINUS | DPLUS);
	//set to output
	DDR |= (DMINUS | DPLUS);
}

void USB::connect()
{
	using namespace USB;
	//set to input
	DDR &= ~(DMINUS | DPLUS);
	//set low : disable pullup
	PORT &= ~(DMINUS | DPLUS);
}

void USB::init(USB::Endpoint0 *endpoint0)
{
	reset();
	//Enable interrupts
	USB::INT_CFG |= USB::INT_CFG_SET;
	USB::INT_ENABLE |= USB::INT_ENABLE_MASK;
	USB::INT_PENDING |= USB::INT_PENDING_MASK;

	//clear global variables
	usbNewDeviceAddr = 0;
	usbDeviceAddr = 0;
	usbCurrentTok = 0;
	usbRxToken = 0;
	usbRxLen = 0;
	usbInputBufOffset = 0;
	
	EndpointsIn[0] = endpoint0;
	EndpointsOut[0] = endpoint0;
	usbTxLenBufs[0] = endpoint0->buf();

	for(uint8_t i = 1; i < MAX_ENDPTS; i++){
		usbTxLenBufs[i] = nullptr;
		EndpointsIn[i] = nullptr;
		EndpointsOut[i] = nullptr;
	}

	//enable global interrupts
	AVR::Interrupt::enable();
}

void USB::reset()
{
	AVR::Interrupt::InterruptHolder hold;
	
	disconnect();
	uint8_t i = 1;
	while(i++){
		_delay_ms(1);
		AVR::Watchdog::reset();
	}

	connect();
}

bool USB::ready()
{
	return static_cast<Endpoint0*>(EndpointsIn[0])->configurationSet();
}

void __vector_transaction()
{
	using namespace USB;
	
	#if USE_HANDLER_LOCK
	static bool locked = false;
	{
		AVR::Interrupt::InterruptHolder<true> hold;
		if(locked) return;
		locked = true;
	}
	#endif

	EndpointOut *endpt = EndpointsOut[usbEndptNoData];
	if(endpt) {
		PID rxToken = static_cast<PID>(usbRxToken);
		uint8_t *buf = &usbRxBuf[USB_BUF_LEN] - usbInputBufOffset;
		bool setup = false;
		if(usbRxLen){
			switch (rxToken)
			{
			case PID::SETUP :
				setup = true;
				[[fallthrough]];
			case PID::OUT :
				endpt->out(buf, usbRxLen, setup);
				break;
			//something bad, so default to consuming data
			default: usbRxLen = 0; break;
			}
		}
	}
	else{
		//something bad, so default to consuming data
		usbRxLen = 0;
	}
	#if USE_HANDLER_LOCK
	locked = false;
	#endif

	//update endpoint TX Buffers
	for(auto endptIn : EndpointsIn)
		if(endptIn) endptIn->in();
}