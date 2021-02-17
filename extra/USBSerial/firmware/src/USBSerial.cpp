#include "../include/USBSerial.hpp"
#include "include/SerialDevice.hpp"

#include <avr/io/io.hpp>
#include <avr/io/serial.hpp>

using namespace AVR::USBSerial;
using namespace AVR::USB;

#define USBSERIAL_ENDPT(num) \
SerialEndpoint AVR::USBSerial::USBSerial##num{	\
	AVR::pgm_ptr{&pgmSerial##num##In}, 							\
	AVR::pgm_ptr{&pgmSerial##num##Out}							\
};											\

#if (NUM_USBSERIAL) >= 1
USBSERIAL_ENDPT(0)
#endif
#if (NUM_USBSERIAL) >= 2
USBSERIAL_ENDPT(1)
#endif
#if (NUM_USBSERIAL) >= 3
USBSERIAL_ENDPT(2)
#endif
#if (NUM_USBSERIAL) >= 4
USBSERIAL_ENDPT(3)
#endif
#if (NUM_USBSERIAL) >= 5
USBSERIAL_ENDPT(4)
#endif
#if (NUM_USBSERIAL) >= 6
USBSERIAL_ENDPT(5)
#endif
#if (NUM_USBSERIAL) >= 7
USBSERIAL_ENDPT(6)
#endif
#if (NUM_USBSERIAL) >= 8
USBSERIAL_ENDPT(7)
#endif
#if (NUM_USBSERIAL) >= 9
USBSERIAL_ENDPT(8)
#endif
#if (NUM_USBSERIAL) >= 10
USBSERIAL_ENDPT(9)
#endif
#if (NUM_USBSERIAL) >= 11
USBSERIAL_ENDPT(10)
#endif
#if (NUM_USBSERIAL) >= 12
USBSERIAL_ENDPT(11)
#endif
#if (NUM_USBSERIAL) >= 13
USBSERIAL_ENDPT(12)
#endif
#if (NUM_USBSERIAL) >= 14
USBSERIAL_ENDPT(13)
#endif
#if (NUM_USBSERIAL) >= 15
USBSERIAL_ENDPT(14)
#endif


bool SerialEndpoint::out(uint8_t *rxBuf, uint8_t &rxLen, bool)
{
	for(uint8_t i = 1; i < rxLen-2; i++){
		if(!m_buf_in.full())
			m_buf_in.push_back(rxBuf[i]);
		if(m_echo)
			m_buf_out.push_back(rxBuf[i]);
	}
	rxLen = 0;
	return true;
}

#pragma GCC push_options
#pragma GCC optimize ("no-optimize-sibling-calls")

void SerialEndpoint::in()
{
	if(txLen()) return;
	int i;
	for(i = 0; i < 8 && !m_buf_out.empty(); i++)
		txBuf()[i] = m_buf_out.pop_front();
	if(i){
		genPacket(getDataPID(), i);
	}
}

#pragma GCC pop_options
