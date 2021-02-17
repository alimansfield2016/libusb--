#pragma once

#include <usb/endpoint.hpp>
#include <buffer.hpp>
#include <avr/io/serial.hpp>

#ifndef NUM_USBSERIAL
#define NUM_USBSERIAL 0
#endif

namespace AVR::USBSerial
{

	class SerialEndpoint final : public AVR::USB::EndpointInOut, public AVR::Serial
	{
	private:
		/**
		 * @brief FIFO ring-buffer to store 
		 * outbound data. Holds at most
		 * 1 usb-interrupt packet worth
		 */
		mutable std::ring_buffer<char, 8> m_buf_out;
		/**
		 * @brief FIFO ring-buffer to store 
		 * inbound data. Holds at most
		 * 2 usb-interrupt packet worth.
		 * With interrupt period@10ms
		 * this should suffice for at least 20ms delay from program
		 */
		mutable std::ring_buffer<char, 16> m_buf_in;
		bool m_echo;
	public:
		SerialEndpoint(AVR::pgm_ptr<AVR::USB::EndpointDescriptor> _descIn, AVR::pgm_ptr<AVR::USB::EndpointDescriptor> _descOut) : AVR::USB::EndpointInOut{_descIn, _descOut} {};
		bool out(uint8_t *rxBuf, uint8_t &rxLen, bool _setup) override;
		void in() override;
	private:
		char getchar() const
		{ 
			//wait for data
			while(m_buf_in.empty()); 
			return m_buf_in.pop_front(); 
		}
		void putchar(char c) const
		{ 
			//wait for space
			while(m_buf_out.full()); 
			m_buf_out.push_back(c); 
		}
	public:
		inline std::size_t available() const volatile override { return m_buf_in.count(); }
		void write(const char *src, std::size_t len) const override
		{
			while(len--)
				putchar(*src++);
		}
		void writePGM(AVR::pgm_ptr<char> src, std::size_t len) const override
		{
			while(len--)
				putchar(*src++);
		}
		void read(char *dst, std::size_t len) const override
		{
			while(len--)
				*dst++ = getchar();
		}
		bool echo() const { return m_echo; }
		void echo(bool val) { m_echo = val; }
	};

	#if (NUM_USBSERIAL) >= 1
	extern SerialEndpoint USBSerial0;
	#endif
	#if (NUM_USBSERIAL) >= 2
	extern SerialEndpoint USBSerial1;
	#endif
	#if (NUM_USBSERIAL) >= 3
	extern SerialEndpoint USBSerial2;
	#endif
	#if (NUM_USBSERIAL) >= 4
	extern SerialEndpoint USBSerial3;
	#endif
	#if (NUM_USBSERIAL) >= 5
	extern SerialEndpoint USBSerial4;
	#endif
	#if (NUM_USBSERIAL) >= 6
	extern SerialEndpoint USBSerial5;
	#endif
	#if (NUM_USBSERIAL) >= 7
	extern SerialEndpoint USBSerial6;
	#endif
	#if (NUM_USBSERIAL) >= 8
	extern SerialEndpoint USBSerial7;
	#endif
	#if (NUM_USBSERIAL) >= 9
	extern SerialEndpoint USBSerial8;
	#endif
	#if (NUM_USBSERIAL) >= 10
	extern SerialEndpoint USBSerial9;
	#endif
	#if (NUM_USBSERIAL) >= 11
	USBSERIAL_ENDPTextern SerialEndpoint USBSerial0;
	#endif
	#if (NUM_USBSERIAL) >= 12
	USBSERIAL_ENDPTextern SerialEndpoint USBSerial1;
	#endif
	#if (NUM_USBSERIAL) >= 13
	USBSERIAL_ENDPTextern SerialEndpoint USBSerial2;
	#endif
	#if (NUM_USBSERIAL) >= 14
	USBSERIAL_ENDPTextern SerialEndpoint USBSerial3;
	#endif
	#if (NUM_USBSERIAL) >= 15
	USBSERIAL_ENDPTextern SerialEndpoint USBSerial4;
	#endif

	extern const AVR::USB::Device pgmDevice PROGMEM;

} // namespace AVR::USB::Serial
