#pragma once

#include "usbconfig.h"

#include <avr/io/io.hpp>

#define USB_CONCAT(a, b)            a ## b
#define USB_CONCAT_EXPANDED(a, b)   USB_CONCAT(a, b)

#define USB_OUTPORT(name)           USB_CONCAT(PORT, name)
#define USB_INPORT(name)            USB_CONCAT(PIN, name)
#define USB_DDRPORT(name)           USB_CONCAT(DDR, name)
#define USBOUT          USB_OUTPORT(USB_CFG_IOPORTNAME)
#define USBIN           USB_INPORT(USB_CFG_IOPORTNAME)
#define USBDDR          USB_DDRPORT(USB_CFG_IOPORTNAME)

namespace AVR::USB
{
	constexpr AVR::Register8 DDR{USBDDR};
	constexpr AVR::Register8 PIN{USBIN};	
	constexpr AVR::Register8 PORT{USBOUT};	
	constexpr AVR::BV DMINUS{USB_CFG_DMINUS_BIT};
	constexpr AVR::BV DPLUS{USB_CFG_DPLUS_BIT};

	constexpr AVR::Register8 INT_CFG{USB_INTR_CFG};
	constexpr AVR::Register8 INT_ENABLE{USB_INTR_ENABLE};
	constexpr AVR::Register8 INT_PENDING{USB_INTR_PENDING};
} // namespace AVR::USB

