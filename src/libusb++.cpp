#include <libusb++.hpp>
#include "libusb++_ext.hpp"
#include "endpoint0.hpp"
#include "../usbConfig.hpp"
#include <util/delay.h>

#include <array>

#include <avr/interrupt/interrupt.hpp>
#include <avr/interrupt/watchdog.hpp>

using namespace AVR;

uint8_t usbDeviceAddr;
uint8_t usbNewDeviceAddr;
uint8_t usbTxBuf[11];
volatile uint8_t usbTxLen;
uint8_t usbRxBuf[11];
volatile uint8_t usbRxLen;
uint8_t usbRxToken;
uint8_t usbCurrentTok;
uint8_t usbInputBufOffset;
volatile uint8_t usbEndptNo;
uint8_t *usbTxLenBufs[MAX_ENDPTS];

std::array<USB::Endpoint*, MAX_ENDPTS> Endpoints;

void disconnect()
{
	PORTB &= ~0x02;
	using namespace USB;
	//set low
	PORT &= ~(DMINUS | DPLUS);
	//set to output
	DDR |= DMINUS | DPLUS;
}

void connect()
{
	PORTB |= 0x02;
	using namespace USB;
	//set to input
	DDR &= ~(DMINUS | DPLUS);
	//set low : disable pullup
	PORT &= ~(DMINUS | DPLUS);
}

void USB::init(USB::Endpoint0 *endpoint0)
{
	reset();
	//required pins will now be set.

	//Enable interrupts
	USB::INT_CFG = USB::INT_CFG_SET;
	USB::INT_ENABLE = USB::INT_ENABLE_MASK;
	USB::INT_PENDING = USB::INT_PENDING_MASK;

	//clear global variables
	usbDeviceAddr = 0;
	usbNewDeviceAddr = 0;
	usbCurrentTok = 0;
	usbRxToken = 0;
	usbRxLen = 0;
	usbTxLen = 0;
	usbInputBufOffset = 0;
	

	if(endpoint0)
		Endpoints[0] = endpoint0;
	else
		Endpoints[0] = &_endp0;

	usbTxLenBufs[0] = Endpoints[0]->buf();
	for(uint8_t i = 1; i < MAX_ENDPTS; i++){
		usbTxLenBufs[i] = nullptr;
		Endpoints[i] = nullptr;
	}

	//enable global interrupts
	AVR::Interrupt::enable();
	PINB = 0x01;
	PINB = 0x01;
}

void USB::reset()
{
	AVR::Interrupt::InterruptHolder hold;
	
	disconnect();

	for(uint8_t i = 0; i; ++i){
		_delay_ms(1);
		AVR::Watchdog::reset();
	}

	connect();
}

void __attribute__((interrupt)) handleTransaction()
{
	USB::PORT |= 0x01;
	USB::Endpoint *endpt = Endpoints[usbEndptNo];
	if(!endpt) return;
	USB::PORT &= ~0x01;
}