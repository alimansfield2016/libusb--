#include <usb/libusb++.hpp>
#include <avr/interrupt/interrupt.hpp>

#include "include/SerialDevice.hpp"
#include "include/SerialEndpoint.hpp"

#include <avr/io/serial.hpp>
#include <multiprocess/process.hpp>

using namespace AVR::USBSerial;


int main()
{ 
	USART0.begin(115200);

	AVR::USB::pDevice = &AVR::USBSerial::pgmDevice;
	AVR::USB::init();

	USBSerial0.echo(false);
	USBSerial1.echo(true);

	while(!AVR::USB::ready());



	while(1) USBSerial0.puts(P("Hello World!\r\n"));
}