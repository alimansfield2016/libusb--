#pragma once

#include <stdint.h>

#define MAX_ENDPTS 16
#define USB_BUF_LEN 11

extern "C"{
	extern uint8_t usbDeviceAddr;
	extern uint8_t usbNewDeviceAddr;
	extern uint8_t usbRxBuf[USB_BUF_LEN*2];
	extern uint8_t usbRxLen;
	extern uint8_t usbRxToken;
	extern uint8_t usbCurrentTok;
	extern uint8_t usbInputBufOffset;

	/**
	 * @brief Used to store the endpoint number
	 * at the start of each transaction
	 */
	extern volatile uint8_t usbEndptNoBegin;
	/**
	 * @brief Set using usbEndptNoBegin
	 * at the acceptance of data
	 */
	extern volatile uint8_t usbEndptNoData;
	extern uint8_t *usbTxLenBufs[MAX_ENDPTS];
	extern bool usbTransactionEnd;

	extern void __vector_transaction() __attribute__((signal)) ;
	extern void usbCrc16Append(uint8_t*, uint8_t);
}

#include <array>
#include <usb/endpoint.hpp>

extern std::array<AVR::USB::EndpointOut*, MAX_ENDPTS> EndpointsOut;
extern std::array<AVR::USB::EndpointIn*, MAX_ENDPTS> EndpointsIn;