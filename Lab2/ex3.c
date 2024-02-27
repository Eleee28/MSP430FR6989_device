#include <msp430.h>

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    P9DIR |= BIT7;		// Configure P9.7 (Green LED) as output

	P9OUT |= BIT7;		// Turn on the LED

    while(1) {
	
        P9OUT ^= BIT7;		// Toggle the LED

        // Delay by looping for a certain number of cycles
        volatile unsigned int i;
        for (i = 0; i < 30000; i++) {
            __nop(); // Perform a no-operation
        }
    }

    return 0;
}
