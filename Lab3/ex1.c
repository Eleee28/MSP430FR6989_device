#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    P1DIR |= BIT0;                          // Set P1.0 (Red LED) to output direction

    // Configure P1.1 (Switch) as input with pull-up resistor enabled
    P1DIR &= ~BIT1;                         // Set P1.1 as input
    P1REN |= BIT1;                          // Enable pull-up resistor
    P1OUT |= BIT1;                          // Set pull-up resistor

    while (1) {
        if ((BIT1 & P1IN) == 0) P1OUT |= BIT0;      // If P1.1 is low (pressed) turn on the LED
        else P1OUT &= ~BIT0;                        // If P1.1 is high (not pressed) turn off the LED
    }

    return 0;
}
