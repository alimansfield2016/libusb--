#pragma once

#include <stdint.h>

#define MAX_ENDPTS 16

extern "C"{
	extern uint8_t usbDeviceAddr;
	extern uint8_t usbNewDeviceAddr;
	extern uint8_t usbTxBuf[11];
	extern volatile uint8_t usbTxLen;
	extern uint8_t usbRxBuf[11];
	extern volatile uint8_t usbRxLen;
	extern uint8_t usbRxToken;
	extern uint8_t usbCurrentTok;
	extern uint8_t usbInputBufOffset;

	extern uint8_t *usbTxLenBufs[MAX_ENDPTS];
}