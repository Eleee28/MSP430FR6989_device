#include <msp430.h>

volatile char c = 'A'; // Initialize the first letter to be sent

int main() {

    // Initialize the system
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    // Select the Primary Module Function for Transmission
    P3SEL0 |= BIT4;         // Set to 1
    P3SEL1 &= ~BIT4;        // Set to 0

    // 8 Hz clock signal configuration
    CSCTL0_H = CSKEY >> 8; // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL; // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Set all dividers
    CSCTL0_H = 0; // Lock CS registers


    // eUSCI module configuration for UART mode

    // Configure USCI_A1 for UART mode
    UCA1CTLW0 = UCSWRST; // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK; // CLK = SMCLK

    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1

    UCA1BR0 = 52; // 8000000/16/9600
    UCA1BR1 = 0x00;
    UCA1MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA1CTLW0 &= ~UCSWRST; // Initialize eUSCI

    // Configure interruptions to receive and transmit data in register UCA1IE
    UCA1IE |= UCTXIE;     // Enable the UART transmission interrupts (bit 1 in UCA1IE)

   // _BIS_SR(GIE);         // Enable the global interrupts
    __enable_interrupt();
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    if (UCTXIFG)  // Check if a transmission interrupt has happened
    {
        // Send the next uppercase letter to the hypervisor writing its ASCII code in UCA1TXBUF
        UCA1TXBUF = c; // Load data writing ASCII into buffer
        c++;
        if (c > 'Z') c = 'A'; // Reset alphabet to 'A' after 'Z'
        __delay_cycles(10000); // Add a delay if transmission is too fast
    }
}

