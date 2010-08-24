
#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver/atmega/uart.hpp>

#include <xpcc/driver/apb/interface.hpp>

MAIN_FUNCTION
{
	xpcc::apb::Interface< xpcc::BufferedUart0 > interface;
	
	// set baudrate etc.
	interface.initialize();
	
	// enable interrupts
	sei();
	
	while(1)
	{
		// decode received messages
		interface.update();
		
		// ... TODO ...
	}
}
