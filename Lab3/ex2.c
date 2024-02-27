#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    P1DIR |= BIT0;                          // Set P1.0 to output direction

    // Configure P1.1 (Switch) as input with pull-up resistor enabled
    P1DIR &= ~BIT1;                         // Set P1.1 as input
    P1REN |= BIT1;                          // Enable pull-up resistor
    P1OUT |= BIT1;                          // Set pull-up resistor


    P1IE |= BIT1;                           // Enable interrupt for P1.1 (Switch)
    P1IES |= BIT1;                          // Set interrupt to trigger on falling edge for P1.1 (Switch)
    P1IFG &= ~BIT1;                         // Clear P1.1 (Switch) interrupt flag

    _BIS_SR(GIE);                           // Enable global interrupts

    return 0;
}

/**
 * Preprocessor directive that instructs the compiler to plave the following interrup service routine (ISR)
 * at the interrup vector associated with the PORT1 interrupt.
*/
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    P1OUT ^= BIT0;                  // Toggle the LED (P1.0)
    P1IFG &= ~BIT1;                 // Clear P1.1 (Switch) interrupt flag
}
