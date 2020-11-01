#pragma once

#include <stdint.h>

#define MAX_ENDPTS 16
#define USB_BUF_LEN 11

extern "C"{
	extern uint8_t usbDeviceAddr;
	extern uint8_t usbNewDeviceAddr;
	// extern uint8_t usbTxBuf[11];
	// extern volatile uint8_t usbTxLen;
	extern uint8_t usbRxBuf[USB_BUF_LEN*2];
	extern uint8_t usbRxLen;
	extern uint8_t usbRxToken;
	extern uint8_t usbCurrentTok;
	extern uint8_t usbInputBufOffset;

	extern volatile uint8_t usbEndptNo;
	extern uint8_t *usbTxLenBufs[MAX_ENDPTS];
	extern bool usbTransactionEnd;

	extern void __vector_transaction() __attribute__((interrupt)) ;
}