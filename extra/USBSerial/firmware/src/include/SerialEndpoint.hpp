#pragma once

#include <usb/endpoint.hpp>
#include <buffer.hpp>
#include <avr/io/serial.hpp>
// #include <streambuf>

namespace AVR::USBSerial
{

	using namespace AVR::USB;
	class SerialEndpoint final : public EndpointInOut, public AVR::Serial
	{
	public:
		/**
		 * @brief FIFO ring-buffer to store 
		 * outbound data. Holds at most
		 * 1 usb-interrupt packet worth
		 */
		mutable std::ring_buffer<char, 8> m_buf_out;
		/**
		 * @brief FIFO ring-buffer to store 
		 * inbound data. Holds at most
		 * 4 usb-interrupt packet worth.
		 * With interrupt period@10ms
		 * this should suffice for at least 40ms delay from program
		 */
		mutable std::ring_buffer<char, 32> m_buf_in;
		bool m_echo;
	public:
		SerialEndpoint(const EndpointDescriptor *_descIn, const EndpointDescriptor *_descOut) : AVR::USB::EndpointInOut{_descIn, _descOut} {};
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
		std::size_t available() const volatile override { return m_buf_in.count(); }
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
		// std::size_t putn(const char* s, std::size_t len) { std::size_t i; for(i = 0; i < len && !m_buf_out.full(); i++) m_buf_out.push_back(s[i]); return i; }
		// std::size_t getn(char* s, std::size_t len) { std::size_t i; for(i = 0; i < len && !m_buf_in.empty(); i++) s[i] = m_buf_out.pop_front(); return i; }
		bool echo() const { return m_echo; }
		void echo(bool val) { m_echo = val; }
	};
	
	
	extern SerialEndpoint USBSerial0;
	extern SerialEndpoint USBSerial1;
	extern SerialEndpoint USBSerial2;
	extern SerialEndpoint USBSerial3;
	extern SerialEndpoint USBSerial4;
	extern SerialEndpoint USBSerial5;
	extern SerialEndpoint USBSerial6;
	extern SerialEndpoint USBSerial7;
	extern SerialEndpoint USBSerial8;
	extern SerialEndpoint USBSerial9;
	extern SerialEndpoint USBSerial10;
	extern SerialEndpoint USBSerial11;
	extern SerialEndpoint USBSerial12;
	extern SerialEndpoint USBSerial13;
	extern SerialEndpoint USBSerial14;


} // namespace AVR::USB::Serial
