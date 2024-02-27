#include <msp430.h>

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    
    P1DIR |= BIT0;		// Configure P1.0 (LED) as output

    while(1) {
        
        P1OUT ^= BIT0;		// Toggle the LED

        __delay_cycles(300000);			// Delay for 300,000 cycles
    }

    return 0;
}
