#pragma once

#include <stdint.h>

namespace AVR::USB
{
	class Endpoint;
	class Endpoint0;

	/**Allows the user to provide a custom Endpoint Zero.
	 * It must however inherit from Endpoint0 and use some of its 
	 * built in functionality
	 */	
	void init(Endpoint0 *endpoint0 = nullptr);
	void reset();

	enum class PID : uint8_t
	{
		//Token
		OUT = 0xE1,
		//Token
		SOF = 0xA5,
		//Token
		IN = 0x69,
		//Token
		SETUP = 0x2D,
		
		//Data
		DATA0 = 0xC3,
		//Data
		DATA1 = 0x4B,
		//Data
		DATA2 = 0x87,
		//Data
		MDATA = 0x0F,

		//Handshake
		ACK = 0xD2,
		//Handshake
		NAK = 0x5A,
		//Handshake
		STALL = 0x1E,
		//Handshake
		NYET = 0x96,

		//Special
		PRE = 0x3C,
		//Special
		ERR = 0x3C,
		//Special
		SPLIT = 0x78,
		//Special
		PING = 0xB4,
		//Special
		RESERVED = 0xF0,
	}; // enum class PID
	
} // namespace AVR::USB
