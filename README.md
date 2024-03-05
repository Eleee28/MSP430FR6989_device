# MSP430FR6989_device
In this repository you will find all the lab practices carried out in the subject of Computer Structures (EC). This labs consist on programming input/output devices. We will be using the MSP430FR6989 board from Texas Instruments and the IDE Code Composer Studio (CCS).

> [!NOTE]
> I will only upload the .c files

## Lab 1

Lab 1 consists on an introduction to both the board and the IDE.

- **ex1**: Run demo 1 (Out-of-Box) and understand it. Modify the messages on the LCD.

- **ex2**: Run demo 2 (Blink LED) and understand it. Modify the blinking frequency.

## Lab 2

Lab 2 consists on configuring and managing some input/output devices of the board.

- **ex1**: Implement a "HELLO WORLD" program in embedded C. The message will be displayed on the Console window of the CCS environment.

- **ex2**: Program a code that configures the red LED (P1.0) as an output, and makes it blink with a certain delay (300.000 cycles) using a loop. Use ```__delay_cycles(300000);```.

- **ex3**: Program a code that configures the green LED (P9.7) as an output, and makes it blink with a certain delay (300.000 cycles) using a loop. Without using ```__delay_cycles(300000);```.

- **ex4**: Program a code that combines both the red and green leds, making them blink at the same time.

- **ex5**: Implement a version of the blinking red led (ex2), but calling predefined functions instead of modifying the bits directly.

## Lab 3

Lab 3 consists on configuring and managing the interrupt system of the board. We will use both polling and interrupts.

- **ex1**: Program an embedded C code that toggles the red led when switch P1.1 is pressed using **polling**.

	- Initialize PM5CTL0.

	- Configure the led as an output.

	- Configure the switch as an input. Configure it as a “pullup”.

	- Do an infinite loop and check if the switch is pressed down (led on) or the switch is not pressed down (led off). <br>

	<b>*Note:*</b> The switch works with “active-low” logic, that means that when you press it
down its bit will be 0, and when you don’t press it down its bit will be 1.

- **ex2**: Program an embedded C code that toggles the red led when switch P1.1 is pressed using interrupts.

	- Initialize as in ex1.

	- Instead of the loop, enable interrupts:

		- Activate bit 1 in P1IE.

		- Program it to detect a High to Low transition (bit 1 in P1IES). This would detect when the button is pressed down.

		- Activate the global interrupts: _BIS_SR(GIE);

		- Add the PORT1_VECTOR ISR function. Inside this ISR, add a line to toggle the output of the red led, and another one to delete the interrupt flag of the switch (bit 1 in P1IFG). 

- **ex3**: Program an embedded C code that toggles the red led using Timer_A interrupts (timer should count up to 40,000 to produce the interrupt).

	- Initialize the counter. Program it to count 40,000 cycles.

	- Add the TIMER1_A0_VECTOR ISR function. Toggle the output of the led each time the timer interrupts.

- **ex4**: Program an embedded C code that increments and shows a counter on the LCD every time the Timer_A causes an interrupt (timer should count up to 40,000 to produce the interrupt). Switches should also be configured to cause interrupts with the following functionality:<br>

	a. Switch 1: Counter is stopped when pressed and continues if pressed again. <br>
	b. Switch 2: Reset the counter to 0. <br>

	- Configure everything as done in previous exercises.

	-  Define a global variable n, that will represent the count value to show on the LCD.

	- Initialize the display using ```Initialize_LCD();```.

	-  You need two ISR, one to service the timer and another one the two switches:

		- Timer: When it interrupts, increase the value of n and show it on the LCD with ```display_num_lcd(n);```.

		- Switches: If switch 1 interrupts, stop / restart the counter. This is achieved toggling bit 4 (i.e. MC_1) in TA1CTL. If switch 2 interrupts,reset the counter by activating bit 2 (i.e. TACLR) in TA1CTL and setting n to 0.

## Lab 4

Lab 4 consists on configuring and managing the serial communication of the board.

- **ex1**: Program an embedded C code that uses the UART to continuously transmit the uppercase alphabet with the following configuration: 9600 baud rate, 8-bit data, 1-bit stop, and no parity. The alphabet must be shown on the hyperterminal window.

	- Initialize the system as in previous projects.
	
	- Select the Primary Module Function for Transmission (set bit 4 in P3SEL0 to 1 and bit 4 in P3SEL1 to 0).

	- Launch the 8-Hz clock signal configuration.

	- Launch the eUSCI module configuration for UART mode.

	- Enable the UART transmission interrupts (bit 1 in UCA1IE).

	- Enable the global interrupts.

	- Add the USCI_A1_VECTOR ISR for the UART. Inside the ISR, check if a transmission interrupt has happened (bit 1 in UCTXIFG). If so, send the next uppercase letter to the hypervisor writing its ASCII code in UCA1TXBUF. Only one letter is sent per transmission interrupt.

	- After sending "Z", the next letter will be "A", for a continuous transmission. If the transmission is too fast, try to add a delay after each letter is transmitted.

- **ex2**: Program an embedded C code to transmit uppercase letters from the hyperterminal to the UART with the following configuration: 9600 baud rate, 8-bit data, 1-bit stop, and no parity. The received letters must be shown on the MSP430 LCD. The LCD must display the last 6 letters using the function ShowBuffer.

	- Do a configuration similar to the one of program 1, but now configuring the system for Reception instead of Transmission (modify bit 5 in P3SEL0 and P3SEL1, bit 0 in UCA1IE, and check bit 0 in UCTXIFG).

	- Add the USCI_A1_VECTOR ISR for the UART. Inside the ISR, check if a reception interrupt has happened. If so, that means that a key has been pressed on your PC keyboard. The ASCII code of the pressed key can be found in the UCA1RXBUF register. 

	- You need to create a function ``void ShiftBuffer(int buffer[], int m)``, that works on a predefined 6-element buffer (array). Each buffer position will correspond to one of the 6 displays on the LCD. When an interrupt happens, you need to call this function, being m the ASCII code that has been pressed. The function will shift the content of the buffer one position to the left. In this way, the leftmost value will be lost, and the pressed ASCII code has to be written in the rightmost position. 
	
	- After this, you have to call the ``void ShowBuffer(int buffer[])`` to show the content of the buffer on the LCD. For this function to work, you also have to add the const char alphabetBig[26][2] structure.

- **ex3**: Program an embedded C code that combines the two previous functionalities, so that it transmits and receives data between the UART and the hyperterminal.

	- Combine the initialization for transmission and reception.

	- You just need one ISR to service both transmission and reception.
	
	- Inside the ISR, check if the interrupt was produced by the reception or the transmission, and add the code of the two previous programs to each of the cases.
