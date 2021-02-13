/* Name: asmcommon.inc
 * Project: V-USB, virtual USB port for Atmel's(r) AVR(r) microcontrollers
 * Author: Christian Starkjohann
 * Creation Date: 2007-11-05
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 */

/* Do not link this file! Link usbdrvasm.S instead, which includes the
 * appropriate implementation!
 */

/*
General Description:
This file contains assembler code which is shared among the USB driver
implementations for different CPU cocks. Since the code must be inserted
in the middle of the module, it's split out into this file and #included.

Jump destinations called from outside:
    sofError: Called when no start sequence was found.
    se0: Called when a package has been successfully received.
    overflow: Called when receive buffer overflows.
    doReturn: Called after sending data.

Outside jump destinations used by this module:
    waitForJ: Called to receive an already arriving packet.
    sendAckAndReti:
    sendNakAndReti:
    sendCntAndReti:
    usbSendAndReti:

The following macros must be defined before this file is included:
    .macro POP_STANDARD
    .endm
    .macro POP_RETI
    .endm
*/

#define token   x1

overflow:
    ldi     x2, 1<<USB_INTR_PENDING_BIT
    USB_STORE_PENDING(x2)       ; clear any pending interrupts
ignorePacket:
    clr     token
    rjmp    storeTokenAndReturn

;----------------------------------------------------------------------------
; Processing of received packet (numbers in brackets are cycles after center of SE0)
;----------------------------------------------------------------------------
;This is the only non-error exit point for the software receiver loop
;we don't check any CRCs here because there is no time left.
se0:
    subi    cnt, USB_BUFSIZE    ;[5]	count -= 11
    neg     cnt                 ;[6]	count = -count
    sub     YL, cnt             ;[7]	buf += (-)-count(-11)
    sbci    YH, 0               ;[8]	Y = usbRxBux + usbRxBufOffset;
    ldi     x2, 1<<USB_INTR_PENDING_BIT ;[9]
    USB_STORE_PENDING(x2)       ;[10] clear pending intr and check flag later. SE0 should be over.
    ld      token, y            ;[11]
    cpi     token, USBPID_DATA0 ;[13]
    breq    handleData          ;[14]
    cpi     token, USBPID_DATA1 ;[15]
    breq    handleData          ;[16]
    lds     shift, usbDeviceAddr;[17]
    ldd     x2, y+1             ;[19] ADDR and 1 bit endpoint number
    lsl     x2                  ;[21] shift out 1 bit endpoint number
    cpse    x2, shift           ;[22]
    rjmp    ignorePacket        ;[23]
/* only compute endpoint number in x3 if required later */
; Since we WILL be dealing with more than one endpoint, 
; we DO want to get the endpoint number
    ldd     x3, y+2             ;[24] endpoint number + crc
    rol     x3                  ;[26] shift in LSB of endpoint
    andi	x3, 0x0F			;[27] 
    cpi     token, USBPID_IN    ;[28]
    breq    handleIn            ;[29]
    cpi     token, USBPID_SETUP ;[30]
    breq    handleSetupOrOut    ;[31]
    cpi     token, USBPID_OUT   ;[32]
    brne    ignorePacket        ;[33] must be ack, nak or whatever
;   rjmp    handleSetupOrOut    ; fallthrough

;Setup and Out are followed by a data packet two bit times (16 cycles) after
;the end of SE0. The sync code allows up to 40 cycles delay from the start of
;the sync pattern until the first bit is sampled. That's a total of 56 cycles.
handleSetupOrOut:               ;[36] May start as late as cycle 36
    sts		usbEndptNoBegin, x3	;[36] indicate that this is endpoint x
storeTokenAndReturn:
    sts     usbCurrentTok, token;[38]
doReturn:
    POP_STANDARD                ;[40] 12...16 cycles
    USB_LOAD_PENDING(YL)        ;[52]
    sbrc    YL, USB_INTR_PENDING_BIT;[53] check whether data is already arriving
    rjmp    waitForJ            ;[54] save the pops and pushes -- a new interrupt is already pending
sofError:
pop_reti_handle:
	lds		YL, usbTransactionEnd;
	tst		YL
	breq	noHandle
	clr		YL
	sts		usbTransactionEnd, YL
	POP_RETI
	sei
	rjmp	__vector_transaction
noHandle:
    POP_RETI                    ;macro call
    reti

handleData:
#if USB_CFG_CHECK_CRC
    CRC_CLEANUP_AND_CHECK       ; jumps to ignorePacket if CRC error
#endif
    lds     shift, usbCurrentTok;[18]
    tst     shift               ;[20]
    breq    doReturn            ;[21]
    lds     x2, usbRxLen        ;[22]
    tst     x2                  ;[24]
    brne    sendNakAndReti      ;[25]
; 2006-03-11: The following two lines fix a problem where the device was not
; recognized if usbPoll() was called less frequently than once every 4 ms.
    cpi     cnt, 4              ;[26] zero sized data packets are status phase only -- ignore and ack
    brmi    sendAckAndReti      ;[27] keep rx buffer clean -- we must not NAK next SETUP
#if USB_CFG_CHECK_DATA_TOGGLING
    sts     usbCurrentDataToken, token  ; store for checking by C code
#endif
    sts     usbRxLen, cnt       ;[28] store received data, swap buffers
    sts     usbRxToken, shift   ;[30]
    lds     x2, usbInputBufOffset;[32] swap buffers
    ldi     cnt, USB_BUFSIZE    ;[34]	cnt = USB_BUFSIZE = 11
    sub     cnt, x2             ;[35]	cnt -= usbInputBufOffset
    sts     usbInputBufOffset, cnt;[36] buffers now swapped
								;	usbInputBufOffset = 11-usbInputBufOffset

	lds		x3, usbEndptNoBegin	;[38]
	sts		usbEndptNoData, x3	;[40]

    rjmp    sendAckAndReti      ;[42] 44 + 19 = 63 until SOP. 
	; WERE CURRENTLY 7 CYCLES OVER SPEC!

handleIn:						;[33]
	sts		usbCurrentTok, token;[33] 
    lsl		x3					;[35] double EndpointNo as uint8_t*[] index
    clr		x2					;[36]
    ldi		YL, lo8(usbTxLenBufs);[37] Load usbTxLenBufs pointer
    ldi		YH, hi8(usbTxLenBufs);[38] into Y
    add		YL, x3				;[39]
    adc		YH, x2				;[40] index into usbTxLenBufs
    ld		x1, y+				;[41]
    ld		YH, y				;[43]
    mov		YL, x1				;[44] de-reference y

    ld		cnt, y				;[45]
    tst		cnt					;[46] if zero, send NAK
    breq	sendNakAndReti		;[47]
    st		y+, x2				;[48] set txLen = 0, increment y to point to start of txBuf

	ldi		x1, 1				;[50]
	sts		usbTransactionEnd, x1;[51]

    inc		cnt					;[53]
    rjmp	usbSendAndReti		;[54]
    
