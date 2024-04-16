#include <msp430.h>

char c = 'A'; // Initialize the first letter to be sent
int buffer[6] = {0}; // Initialize the buffer

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
    LCDCCTL0 = LCDDIV_1 | LCDPRE_16 | LCD4MUX | LCDLP;

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

//function that displays any 16-bit unsigned integer***********
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


const char alphabetBig[26][2] =
{
    {0xEF, 0x00}, /* "A" LCD segments a+b+c+e+f+g+m */
    {0xF1, 0x50}, /* "B" */
    {0x9C, 0x00}, /* "C" */
    {0xF0, 0x50}, /* "D" */
    {0x9F, 0x00}, /* "E" */
    {0x8F, 0x00}, /* "F" */
    {0xBD, 0x00}, /* "G" */
    {0x6F, 0x00}, /* "H" */
    {0x90, 0x50}, /* "I" */
    {0x78, 0x00}, /* "J" */
    {0x0E, 0x22}, /* "K" */
    {0x1C, 0x00}, /* "L" */
    {0x6C, 0xA0}, /* "M" */
    {0x6C, 0x82}, /* "N" */
    {0xFC, 0x00}, /* "O" */
    {0xCF, 0x00}, /* "P" */
    {0xFC, 0x02}, /* "Q" */
    {0xCF, 0x02}, /* "R" */
    {0xB7, 0x00}, /* "S" */
    {0x80, 0x50}, /* "T" */
    {0x7C, 0x00}, /* "U" */
    {0x0C, 0x28}, /* "V" */
    {0x6C, 0x0A}, /* "W" */
    {0x00, 0xAA}, /* "X" */
    {0x00, 0xB0}, /* "Y" */
    {0x90, 0x28} /* "Z" */
};

void ShowBuffer(int buffer[]) {
    LCDMEM[9] = alphabetBig[(buffer[0])-65][0];
    LCDMEM[10] = alphabetBig[(buffer[0])-65][1];
    LCDMEM[5] = alphabetBig[(buffer[1])-65][0];
    LCDMEM[6] = alphabetBig[(buffer[1])-65][1];
    LCDMEM[3] = alphabetBig[(buffer[2])-65][0];
    LCDMEM[4] = alphabetBig[(buffer[2])-65][1];
    LCDMEM[18] = alphabetBig[(buffer[3])-65][0];
    LCDMEM[19] = alphabetBig[(buffer[3])-65][1];
    LCDMEM[14] = alphabetBig[(buffer[4])-65][0];
    LCDMEM[15] = alphabetBig[(buffer[4])-65][1];
    LCDMEM[7] = alphabetBig[(buffer[5])-65][0];
    LCDMEM[8] = alphabetBig[(buffer[5])-65][1];
}

#include <msp430.h>

int main() {

    // Initialize the system
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    // Select the Primary Module Function for Transmission
    P3SEL0 |= BIT4;         // Set to 1
    P3SEL1 &= ~BIT4;        // Set to 0
    // Select the Primary Module Function for Reception
    P3SEL0 |= BIT5;         // Set to 1
    P3SEL1 &= ~BIT5;        // Set to 0

    // 8 Hz clock signal configuration
    CSCTL0_H = CSKEY >> 8; // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL; // Set DCO to 8MHz
    CSCTL2 = SELA_VLOCLK | SELSDCOCLK | SELM_DCOCLK;
    CSCTL3 = DIVA_1 | DIVS1 | DIVM_1; // Set all dividers
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
    UCA1IE |= BIT1;     // Enable the UART transmission interrupts (bit 1 in UCA1IE)
    // Configure interruptions to receive and transmit data in register UCA1IE
    UCA1IE |= BIT0;     // Enable the receive interrupt

    Initialize_LCD();

    _BIS_SR(GIE);         // Enable the global interrupts



}

void ShiftBuffer(int buffer[], int m) {
    int i;
    for (i = 0; i < 5; i++) {
        buffer[i] = buffer[i + 1];
    }
    buffer[5] = m;
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    if (UCA1IFG & UCTXIFG)  // Check if a transmission interrupt has happened
    {
        // Send the next uppercase letter to the hypervisor writing its ASCII code in UCA1TXBUF
        c = c + 1;
        if (c > 'Z') c = 'A'; // Reset alphabet to 'A' after 'Z'
        UCA1TXBUF = c; // Load data into TX buffer
        __delay_cycles(10); // Add a delay if transmission is too fast
    }

    if (UCA1IFG & UCRXIFG)  // Check if a reception interrupt has happened
    {
        ShiftBuffer(buffer, UCA1RXBUF);
        ShowBuffer(buffer);   // Display the new content of the buffer on
     }
}
