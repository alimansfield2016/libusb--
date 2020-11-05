#pragma once

#include <stdint.h>
// #include <avr/memory/pgmspace.hpp>

namespace AVR::USB
{
	class _Endpoint;
	class Endpoint0;

	class Device;
	//PROGMEM
	extern const Device* pDevice;

	/**Allows the user to provide a custom Endpoint Zero.
	 * It must however inherit from Endpoint0 and use some of its 
	 * built in functionality
	 */	
	void init(Endpoint0 *endpoint0 = nullptr);
	void reset();

	void connect();
	void disconnect();

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
	

	enum class RequestDirection : bool{
		HostToDevice = false,
		DeviceToHost = true,
	};

	enum class RequestType : uint8_t{
		Standard = 0,
		Class = 1,
		Vendor = 2,
		RESERVED = 3,
	};

	enum class RequestRecipient : uint8_t{
		Device = 0,
		Interface = 1,
		Endpoint = 2,
		Other = 3,
	};

	enum class DescriptorType : uint8_t{
		Device = 1,
		Configuration = 2,
		String = 3,
		Interface = 4,
		Endpoint = 5,
		DeviceQualifier = 6,
		OtherSpeedConfiguration = 7,
		InterfacePower = 8,
		OnTheGo = 9,
		Debug = 10,
	};


	enum class Request
	{
		GetStatus = 0,
		ClearFeature = 1,
		SetFeature = 3,
		SetAddress = 5,
		GetDescriptor = 6,
		SetDescriptor = 7,
		GetConfiguration = 8,
		SetConfiguration = 9,
		GetInterface = 10,
		setInterface = 11,
		SynchFrame = 18,
	};

	enum class DeviceClass : uint8_t{
		UseInterfaceClass = 0x00,
		CDC_Control = 0x02,
		Hub = 0x09,
		Billboard = 0x11,
		DiagnosticsDevice = 0xDC,
		Miscellaneous = 0xEF,
		VendorSpecific = 0xFF,
	};
	enum class InterfaceClass: uint8_t{
		NONE = 0,
		Audio = 0x01,
		CDC_Control = 0x02,
		HID = 0x03,
		Physical = 0x05,
		Image = 0x06,
		Printer = 0x07,
		MassStorage = 0x08,
		CDC_Data = 0x0A,
		SmartCard = 0x0B,
		ContentSecurity = 0x0D,
		Video = 0x0E,
		PersonalHealthcare = 0x0F,
		AudioVideo = 0x10,
		USBC_Bridge = 0x12,
		DiagnosticsDevice = 0xDC,
		WirelessController = 0xE0,
		Miscellaneous = 0xEF,
		ApplicationSpecific = 0xFE,
		VendorSpecific = 0xFF,
	};

	enum class USB_BCD : uint16_t{
		USB1_0 = 0x0100,
		USB1_1 = 0x0110,
		USB2_0 = 0x0200,
	};

	enum class TransferType : uint8_t{
		Control = 0b00,
		Isochronous = 0b01,
		Bulk = 0b10,
		Interrupt = 0b11,
	};

	enum class ConfigurationAttributes : uint8_t{
		NONE = 0x80,
		RemoteWakeup = 0xA0,
		SelfPowered = 0xC0,
		SelfPowered_RemoteWakeup = 0xE0,
	};


	enum class EndpointDirection : bool{
		OUT = 0,
		IN = 1
	};

	enum class EndpointTransferType : uint8_t{
		Control = 0x00,
		Isochronous = 0x01,
		Bulk = 0x02,
		Interrupt = 0x03,
	};

	enum class EndpointSynchronisationType : uint8_t{
		None = 0x00,
		Async = 0x01,
		Adaptive = 0x02,
		Sync = 0x03,
	};
	
	enum class EndpointUsageType : uint8_t{
		Data = 0x00,
		Feedback = 0x01,
		ImplicitFeedback = 0x02,
		Reserved = 0x03,
	};
	

	class Power{
		const uint8_t val;
	public:
		constexpr Power(uint16_t power) : val{static_cast<uint8_t>(power>>1)} {}
		constexpr operator uint8_t() { return val; }
	};
	constexpr Power operator""_mA(unsigned long long power) { return Power(power); }

} // namespace AVR::USB
