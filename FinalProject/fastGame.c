#include <msp430.h>
#include <string.h>

// Change based on LCD Memory locations
#define pos1 9  /* Digit A1 begins at S18 */
#define pos2 5  /* Digit A2 begins at S10 */
#define pos3 3  /* Digit A3 begins at S6  */
#define pos4 18 /* Digit A4 begins at S36 */
#define pos5 14 /* Digit A5 begins at S28 */
#define pos6 7  /* Digit A6 begins at S14 */

#define NONE 0
#define WIN1 1
#define WIN2 2

int buffer[6] = {65, 65, 65, 65, 65, 65};
int i = 0;

int p1Points = 0;
int p2Points = 0;

int winner = NONE;

// LCD memory map for numeric digits
const char digit[10][2] =
    {
        {0xFC, 0x28}, /* "0" LCD segments a+b+c+d+e+f+k+q */
        {0x60, 0x20}, /* "1" */
        {0xDB, 0x00}, /* "2" */
        {0xF3, 0x00}, /* "3" */
        {0x67, 0x00}, /* "4" */
        {0xB7, 0x00}, /* "5" */
        {0xBF, 0x00}, /* "6" */
        {0xE4, 0x00}, /* "7" */
        {0xFF, 0x00}, /* "8" */
        {0xF7, 0x00}  /* "9" */
};

const char alphabetBig[26][2] = {
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
    {0x90, 0x28}  /* "Z" */
};

void bhzclock()
{
    CSCTL0_H = CSKEY >> 8;        // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL; // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Set all dividers
    CSCTL0_H = 0;                         // Lock CS registers
}

void eUSCIconf()
{
    // Configure USCI_A1 for UART mode
    UCA1CTLW0 = UCSWRST;        // Put eUSCI in reset
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
}

void display_num_lcd(unsigned int nA);
void Initialize_LCD();
void ShowBuffer(int buffer[]);
void ShiftBuffer(int buffer[], int m);

void displayScrollText(char *msg);
void showChar(char c, int position);
void clearLCD();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0; // Set P1.0 to output direction (RED LED) -> for debugging
    P9DIR |= BIT7; // Set P9.7 to output direction (GREEN LED) -> for debugging

    P1DIR &= ~(BIT1 | BIT2); // Set P1.1 and P1.2 to input direction

    // Switch 1: P1.1
    P1OUT |= BIT1; // Set pull-up resistor
    P1REN |= BIT1; // Enable pull-up resistor

    // Enable interrupt for switch 1
    P1IE |= BIT1;   // Enable interrupt for P1.1 (Switch)
    P1IES |= BIT1;  // Set interrupt to trigger on falling edge for P1.1 (Switch)
    P1IFG &= ~BIT1; // Clear P1.1 (Switch) interrupt flag

    // Switch 2: P1.2
    P1OUT |= BIT2; // Set pull-up resistor
    P1REN |= BIT2; // Enable pull-up resistor

    // Enable interrupt for switch 2
    P1IE |= BIT2;   // Enable interrupt for P1.2 (Switch)
    P1IES |= BIT2;  // Set interrupt to trigger on falling edge for P1.2 (Switch)
    P1IFG &= ~BIT2; // Clear P1.2 (Switch) interrupt flag

    P1OUT &= ~BIT0; // Clear red LED
    P9OUT &= ~BIT7; // Clear green LED

    bhzclock();
    eUSCIconf();

    _BIS_SR(GIE);
    Initialize_LCD();

    clearLCD();
    displayScrollText("WELCOME TO THE FAST GAME");

    while (1)
    {

        displayScrollText("3  2  1  PLAY");

        while (winner == NONE)
        {
            display_num_lcd((1000 * p1Points) + p2Points);
            if (p1Points == 10)
                winner = WIN1;
            if (p2Points == 10)
                winner = WIN2;
        }

        if (winner == WIN1)
            displayScrollText("PLAYER 1 WINS");
        else if (winner == WIN2)
            displayScrollText("PLAYER 2 WINS");

        winner = NONE;
        p1Points = 0;
        p2Points = 0;
    }
}

/*
 * Scrolls input string across LCD screen from left to right
 */
void displayScrollText(char *msg)
{
    int length = strlen(msg);
    int i;
    int s = 5;
    char buffer[6] = "      ";
    for (i = 0; i < length + 7; i++)
    {
        int t;
        for (t = 0; t < 6; t++)
            buffer[t] = ' ';
        int j;
        for (j = 0; j < length; j++)
        {
            if (((s + j) >= 0) && ((s + j) < 6))
                buffer[s + j] = msg[j];
        }
        s--;

        showChar(buffer[0], pos1);
        showChar(buffer[1], pos2);
        showChar(buffer[2], pos3);
        showChar(buffer[3], pos4);
        showChar(buffer[4], pos5);
        showChar(buffer[5], pos6);

        __delay_cycles(8 * 200000);
    }
}

/*
 * Displays input character at given LCD digit/position
 * Only spaces, numeric digits, and uppercase letters are accepted characters
 */
void showChar(char c, int position)
{
    if (c == ' ')
    {
        // Display space
        LCDMEM[position] = 0;
        LCDMEM[position + 1] = 0;
    }
    else if (c >= '0' && c <= '9')
    {
        // Display digit
        LCDMEM[position] = digit[c - 48][0];
        LCDMEM[position + 1] = digit[c - 48][1];
    }
    else if (c >= 'A' && c <= 'Z')
    {
        // Display alphabet
        LCDMEM[position] = alphabetBig[c - 65][0];
        LCDMEM[position + 1] = alphabetBig[c - 65][1];
    }
    else
    {
        // Turn all segments on if character is not a space, digit, or uppercase letter
        LCDMEM[position] = 0xFF;
        LCDMEM[position + 1] = 0xFF;
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT1) // Player 1
    {
        p1Points++;
        P1OUT ^= BIT0;  // Toggle red LED
        P1IFG &= ~BIT1; // Clear P1.1 (Switch) interrupt flag
    }
    if (P1IFG & BIT2) // Player 2
    {
        p2Points++;
        P9OUT ^= BIT7;  // Toggle green LED
        P1IFG &= ~BIT2; // Clear P1.2 (Switch) interrupt flag
    }

    __delay_cycles(300000);
    P1OUT &= ~BIT0; // Clear red LED
    P9OUT &= ~BIT7; // Clear green LED
}

void ShowBuffer(int buffer[])
{
    LCDMEM[9] = alphabetBig[(buffer[0]) - 65][0];
    LCDMEM[10] = alphabetBig[(buffer[0]) - 65][1];
    LCDMEM[5] = alphabetBig[(buffer[1]) - 65][0];
    LCDMEM[6] = alphabetBig[(buffer[1]) - 65][1];
    LCDMEM[3] = alphabetBig[(buffer[2]) - 65][0];
    LCDMEM[4] = alphabetBig[(buffer[2]) - 65][1];
    LCDMEM[18] = alphabetBig[(buffer[3]) - 65][0];
    LCDMEM[19] = alphabetBig[(buffer[3]) - 65][1];
    LCDMEM[14] = alphabetBig[(buffer[4]) - 65][0];
    LCDMEM[15] = alphabetBig[(buffer[4]) - 65][1];
    LCDMEM[7] = alphabetBig[(buffer[5]) - 65][0];
    LCDMEM[8] = alphabetBig[(buffer[5]) - 65][1];
}

void ShiftBuffer(int buffer[], int m)
{
    for (i = 0; i < 5; ++i)
    {
        buffer[i] = buffer[i + 1];
    }
    i = 0;
    buffer[5] = m;
}

void Initialize_LCD()
{
    PJSEL0 = BIT4 | BIT5; // For LFXT
    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;
    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8;
    // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;
    // Enable LFXT

    do
    {
        CSCTL5 &= ~LFXTOFFG;
        // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);

    // Test oscillator fault flag
    CSCTL0_H = 0; // Lock CS registers
    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;
    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
    LCDCCPCTL = LCDCPCLKSYNC;
    // Clock synchronization enabled
    LCDCMEMCTL = LCDCLRM;
    // Clear LCD memory
    // Turn LCD on
    LCDCCTL0 |= LCDON;
    return;
}

void display_num_lcd(unsigned int nA)
{
    // initialize i to count though input paremter from main function, digit is used for while loop so as long as n is not 0 the if statements will be executed.
    const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0xE0, 0xFF, 0xE7};
    int i;
    int digit;
    digit = nA;
    while (digit != 0)
    {
        digit = digit * 10;
        i++;
    }
    if (i > 1000)
    {
        LCDM8 = LCD_Num[nA % 10]; // inputs the first(least significant digit) from the array onto the LCD output.
        nA = nA / 10;
        i++;
    }
    if (i > 100)
    {
        LCDM15 = LCD_Num[nA % 10]; // inputs the second(least significant digit) from the array onto the LCD output.
        nA = nA / 10;
        i++;
    }
    if (i > 10)
    {
        LCDM19 = LCD_Num[nA % 10]; // inputs the third(least significant digit) from the array onto the LCD output.
        nA = nA / 10;
        i++;
    }
    if (i > 1)
    {
        LCDM4 = LCD_Num[nA % 10]; // inputs the fourth(least significant digit) from the array onto the LCD output.
        nA = nA / 10;
        i++;
    }
    if (i > 0)
    {
        LCDM6 = LCD_Num[nA % 10]; // inputs the last (most significant digit) from the array onto the LCD output.
        nA = nA / 10;
        i++;
    }
}

/*
 * Clears memories to all 6 digits on the LCD
 */
void clearLCD()
{
    LCDMEM[pos1] = LCDBMEM[pos1] = 0;
    LCDMEM[pos1 + 1] = LCDBMEM[pos1 + 1] = 0;
    LCDMEM[pos2] = LCDBMEM[pos2] = 0;
    LCDMEM[pos2 + 1] = LCDBMEM[pos2 + 1] = 0;
    LCDMEM[pos3] = LCDBMEM[pos3] = 0;
    LCDMEM[pos3 + 1] = LCDBMEM[pos3 + 1] = 0;
    LCDMEM[pos4] = LCDBMEM[pos4] = 0;
    LCDMEM[pos4 + 1] = LCDBMEM[pos4 + 1] = 0;
    LCDMEM[pos5] = LCDBMEM[pos5] = 0;
    LCDMEM[pos5 + 1] = LCDBMEM[pos5 + 1] = 0;
    LCDMEM[pos6] = LCDBMEM[pos6] = 0;
    LCDMEM[pos6 + 1] = LCDBMEM[pos6 + 1] = 0;

    LCDM14 = LCDBM14 = 0x00;
    LCDM18 = LCDBM18 = 0x00;
    LCDM3 = LCDBM3 = 0x00;
}