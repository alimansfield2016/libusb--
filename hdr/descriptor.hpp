#pragma once

#include <endpoint.hpp>

namespace AVR::USB
{
	class Descriptor
	{
		public:
		const uint8_t *const m_ptr;
		uint8_t bLength() const { return m_ptr[0]; };
		DescriptorType bDescriptorType() const { return static_cast<DescriptorType>(m_ptr[1]); };
		protected:
		constexpr Descriptor(const uint8_t *const ptr) : m_ptr(ptr) 
		{
		}
	};

	class DeviceDescriptor : public Descriptor
	{
		const uint8_t m_buf[18];
		public:
		constexpr DeviceDescriptor(
				USB_BCD _bcdUSB, 
				DeviceClass _bDeviceClass,
				uint8_t _bDeviceSubClass,
				uint8_t _bDeviceProtocol,
				uint8_t _bMaxPacketSize0,
				uint16_t _idVendor,
				uint16_t _idProduct,
				uint16_t _bcdDevice,
				uint8_t _iManufacturer,
				uint8_t _iProduct,
				uint8_t _iSerialNumber,
				uint8_t _bNumConfigurations) : 
			Descriptor(m_buf),
			m_buf{
				static_cast<uint8_t>(0x12),
				static_cast<uint8_t>(DescriptorType::Device),
				static_cast<uint8_t>(static_cast<uint16_t>(_bcdUSB)&0xFF),
				static_cast<uint8_t>(static_cast<uint16_t>(_bcdUSB)>>8),
				static_cast<uint8_t>(_bDeviceClass),
				static_cast<uint8_t>(_bDeviceSubClass),
				static_cast<uint8_t>(_bDeviceProtocol),
				static_cast<uint8_t>(_bMaxPacketSize0),
				static_cast<uint8_t>(_idVendor&0xFF),
				static_cast<uint8_t>(_idVendor>>8),
				static_cast<uint8_t>(_idProduct&0xFF),
				static_cast<uint8_t>(_idProduct>>8),
				static_cast<uint8_t>(_bcdDevice&0xFF),
				static_cast<uint8_t>(_bcdDevice>>8),
				static_cast<uint8_t>(_iManufacturer),
				static_cast<uint8_t>(_iProduct),
				static_cast<uint8_t>(_iSerialNumber),
				static_cast<uint8_t>(_bNumConfigurations),
			} {}
		USB_BCD bcdUSB() { return static_cast<USB_BCD>(m_buf[2] | m_buf[3]<<8); }
		DeviceClass bDeviceClass() { return static_cast<DeviceClass>(m_buf[4]); }
		uint8_t bSubDeviceClass() { return static_cast<uint8_t>(m_buf[5]); }
		uint8_t bDeviceProtocol() { return static_cast<uint8_t>(m_buf[6]); }
		uint8_t bMaxPacketSize0() { return static_cast<uint8_t>(m_buf[7]); }
		uint16_t idVendor() { return static_cast<uint16_t>(m_buf[8] | m_buf[9]<<8); }
		uint16_t idProduct() { return static_cast<uint16_t>(m_buf[10] | m_buf[11]<<8); }
		uint16_t bcdDevice() { return static_cast<uint16_t>(m_buf[12] | m_buf[13]<<8); }
		uint8_t iManufacturer() { return static_cast<uint8_t>(m_buf[14]); }
		uint8_t iProduct() { return static_cast<uint8_t>(m_buf[15]); }
		uint8_t iSerialNumber() { return static_cast<uint8_t>(m_buf[16]); }
		uint8_t numConfigurations() { return static_cast<uint8_t>(m_buf[17]); }
	};

	class ConfigurationDescriptor : public Descriptor
	{
		const uint8_t m_buf[9];
	public:
		constexpr ConfigurationDescriptor(
				uint16_t _wTotalLength,
				uint8_t _bNumInterfaces,
				uint8_t _bConfigurationValue,
				uint8_t _iConfiguration,
				ConfigurationAttributes _bmAttributes,
				Power _bMaxPower ) : 
			Descriptor(m_buf),
			m_buf{
				static_cast<uint8_t>(0x09),
				static_cast<uint8_t>(DescriptorType::Configuration),
				static_cast<uint8_t>(_wTotalLength&0xFF),
				static_cast<uint8_t>(_wTotalLength>>8),
				static_cast<uint8_t>(_bNumInterfaces),
				static_cast<uint8_t>(_bConfigurationValue),
				static_cast<uint8_t>(_iConfiguration),
				static_cast<uint8_t>(_bmAttributes),
				static_cast<uint8_t>(_bMaxPower),
			} {}
		uint16_t wTotalLength() const { return static_cast<uint16_t>(m_buf[2] | m_buf[3]<<8); };
		uint8_t bNumInterfaces() const { return static_cast<uint8_t>(m_buf[4]); };
		uint8_t bConfigurationValue() const { return static_cast<uint8_t>(m_buf[5]); };
		uint8_t iConfiguration() const { return static_cast<uint8_t>(m_buf[6]); };
		uint8_t bmAttributes() const { return static_cast<uint8_t>(m_buf[7]); };
		Power bMaxPower() const { return static_cast<Power>(m_buf[8]); };
	};

	class InterfaceDescriptor : public Descriptor
	{
		const uint8_t m_buf[9];
	public:
		constexpr InterfaceDescriptor(
				uint8_t _bInterfaceNumber,
				uint8_t _bAlternateSetting,
				uint8_t _bNumEndpoints,
				InterfaceClass _bInterfaceClass,
				uint8_t _bInterfaceSubClass,
				uint8_t _bInterfaceProtocol,
				uint8_t _iInterface
			) :
			Descriptor(m_buf),
			m_buf{
				static_cast<uint8_t>(0x09),
				static_cast<uint8_t>(DescriptorType::Interface),
				static_cast<uint8_t>(_bInterfaceNumber),
				static_cast<uint8_t>(_bAlternateSetting),
				static_cast<uint8_t>(_bNumEndpoints),
				static_cast<uint8_t>(_bInterfaceClass),
				static_cast<uint8_t>(_bInterfaceSubClass),
				static_cast<uint8_t>(_bInterfaceProtocol),
				static_cast<uint8_t>(_iInterface),
			} {}
		uint8_t bInterfaceNumber() { return static_cast<uint8_t>(m_buf[2]); }
		uint8_t bAlternateSetting() { return static_cast<uint8_t>(m_buf[3]); }
		uint8_t bNumEndpoints() { return static_cast<uint8_t>(m_buf[4]); }
		uint8_t bInterfaceClass() { return static_cast<uint8_t>(m_buf[5]); }
		uint8_t bInterfaceSubClass() { return static_cast<uint8_t>(m_buf[6]); }
		uint8_t bInterfaceProtocol() { return static_cast<uint8_t>(m_buf[7]); }
		uint8_t iInterface() { return static_cast<uint8_t>(m_buf[8]); }
	};

	class EndpointDescriptor : public Descriptor
	{
		
	};

	class StringDescriptor : public Descriptor
	{
	public:
		constexpr StringDescriptor() :
			Descriptor(nullptr) {}
	};

} // namespace AVR::USB
