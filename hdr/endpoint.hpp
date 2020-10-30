#pragma once

#include <libusb++.hpp>
#include <stdint.h>

namespace AVR::USB
{
	class Endpoint;

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
		Audio = 0x01,
		CDC_Control = 0x02,
		HID = 0x03,
		Physical = 0x05,
		Image = 0x06,
		Printer = 0x07,
		MassStorage = 0x08,
		Hub = 0x09,
		CDC_Data = 0x0A,
		SmartCard = 0x0B,
		ContentSecurity = 0x0D,
		Video = 0x0E,
		PersonalHealthcare = 0x0F,
		AudioVideo = 0x10,
		Billboard = 0x11,
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

	class Endpoint
	{
		uint8_t txLenBuf[12];
		PID DataPID;
	protected:
		uint8_t *txBuf;
		void genPacket(PID pid, uint8_t dataLen);
		PID getDataPID() { PID pid = DataPID; if(DataPID == PID::DATA0) DataPID = PID::DATA1; else DataPID = PID::DATA0; return pid; }
		void setDataPID(PID pid) { DataPID = pid; }
	public:
		Endpoint();
		uint8_t *buf() { return txLenBuf; }
		uint8_t txLen() const { return txLenBuf[0]; }
		virtual void setup(uint8_t *rxBuf, uint8_t &rxLen);
		virtual void out(uint8_t *rxBuf, uint8_t &rxLen);
		virtual void in();
	};
} // namespace AVR::USB
