#include <msp430.h>

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Configure P1.0 (Red LED) and P9.7 (Green LED) as outputs
    P1DIR |= BIT0;
    P9DIR |= BIT7;

	// Turn on both LEDs
	P1OUT |= BIT0;
	P9OUT |= BIT7;		

    while(1) {
        // Toggle both LEDs
        P1OUT ^= BIT0;
        P9OUT ^= BIT7;

        __delay_cycles(300000);	// Delay for 300,000 cycles
    }

    return 0;
}
