#include <msp430.h>

int n = 0;          // Global variable n (counter)

//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989�s Sample Code ***
void Initialize_LCD() {
    PJSEL0 = BIT4 | BIT5; // For LFXT

    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers
    CSCTL4 &= ~LFXTOFF; // Enable LFXT

    do {
        CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG); // Test oscillator fault flag

    CSCTL0_H = 0; // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
    LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled
    LCDCMEMCTL = LCDCLRM; // Clear LCD memory
    //Turn LCD on
    LCDCCTL0 |= LCDON;
    return;
}

//***************function that displays any 16-bit unsigned integer************
void display_num_lcd(unsigned int n){
    //initialize i to count though input paremter from main function, digit is used for while loop so as long as n is
   // not 0 the if statements will be executed.

    const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0xE0, 0xFF, 0xE7};
    int i;
    int digit;

    digit = n;

    while(digit!=0) {
        digit = digit*10;
        i++;
    }

    if(i>1000) {
        LCDM8 = LCD_Num[n%10]; // inputs the first(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>100) {
        LCDM15 = LCD_Num[n%10]; // inputs the second(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>10) {
        LCDM19 = LCD_Num[n%10]; // inputs the third(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>1) {
        LCDM4 = LCD_Num[n%10]; // inputs the fourth(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>0) {
        LCDM6 = LCD_Num[n%10]; // inputs the last (most significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
}





int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    // TIMER_A configuration:
    TA1CTL = TASSEL_1 | TACLR | MC_1;       // TimerA1: ACLK/1, reset TACLR, mode up,
    TA1CCR0 = 40000 - 1;                    // Set the count limit to 40000 cycles
    TA1CCTL0 = CCIE;                        // Enable interrupts (CCIE bit)


    P1DIR |= BIT0;                          // Set P1.0 to output direction (RED LED) -> for debugging
    P9DIR |= BIT7;                          // Set P9.7 to output direction (GREEN LED) -> for debugging

    P1DIR &= ~(BIT1 | BIT2);                // Set P1.1 and P1.2 to input direction

    // Switch 1: P1.1
    P1OUT |= BIT1;                          // Set pull-up resistor
    P1REN |= BIT1;                          // Enable pull-up resistor

    // Enable interrupt for switch 1
    P1IE |= BIT1;                           // Enable interrupt for P1.1 (Switch)
    P1IES |= BIT1;                          // Set interrupt to trigger on falling edge for P1.1 (Switch)
    P1IFG &= ~BIT1;                         // Clear P1.1 (Switch) interrupt flag

    // Switch 2: P1.2
    P1OUT |= BIT2;                          // Set pull-up resistor
    P1REN |= BIT2;                          // Enable pull-up resistor

    // Enable interrupt for switch 2
    P1IE |= BIT2;                           // Enable interrupt for P1.2 (Switch)
    P1IES |= BIT2;                          // Set interrupt to trigger on falling edge for P1.2 (Switch)
    P1IFG &= ~BIT2;                         // Clear P1.2 (Switch) interrupt flag

    _BIS_SR(GIE);                           // Enable global interrupts

    Initialize_LCD();

    return 0;
}

// ISR for TIMER1_A0
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void){
    display_num_lcd(n);
    n++;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    if (P1IFG & BIT1) {
        P1OUT ^= BIT0;      // Toggle red LED -> debugging
        TA1CTL ^= MC_1;     // stop/restart counter: toggle bit 4 (MC_1)
        P1IFG &= ~BIT1;     // Clear P1.1 (Switch) interrupt flag
    }

    if (P1IFG & BIT2) {
        P9OUT ^= BIT7;      // Toggle green LED -> deugging
        TA1CTL |= TACLR;    // reset counter: activate bit 2 (TACTL)
        P1IFG &= ~BIT2;     // Clear P1.2 (Switch) interrupt flag
        n = 0;              // reset counter variable
    }

}
