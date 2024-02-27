#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    // TIMER_A configuration:
    TA1CTL = TASSEL_1 | TACLR | MC_1;       // TimerA1: ACLK/1, reset TACLR, mode up,
    TA1CCR0 = 40000 - 1;                    // Set the count limit to 40000 cycles
    TA1CCTL0 = CCIE;                        // Enable interrupts (CCIE bit)

    P1DIR |= BIT0;                          // Set P1.0 to output direction

    _BIS_SR(GIE);                           // Enable global interrupts

    return 0;
}

// ISR for TIMER1_A0
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void){
    P1OUT ^= BIT0;      // toggle LED in P1.0
}
