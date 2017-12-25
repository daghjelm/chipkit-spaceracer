/*
File that configures input and output
Written by Dag Hjelm and Joar Rutqvist 2017
*/
#include <stdint.h>
#include <pic32mx.h>
#include "header.h"
#define TMR2PERIOD ((80000000/256) / 500)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big."
#endif

void configbtn() {

	TRISFSET = 0x2; //Button 1
	TRISDSET = 0xE0; //Button 2,3 & 4

	TRISECLR = 0xFF; //LED's
	PORTE = 0x0;
}

void configscreen() {
	/*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.

	  OBS: All code in this function was
	  taken from mipslabmain.c by Axel Isaksson and F. Lundevall
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */

	//output pins for display signal
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
  	SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

}

void configtimer() {

	TMR2 = 0; //Set TMR2 to be 0, just to make sure
	T2CON = 0x70; //Set 256 bit prescale, 1110000 gives us 256
	T2CONSET = 0x8000; //Start the timer (bit 15 to start timer2)
	PR2 = TMR2PERIOD; //Set the period to be what we defined

}

void configLed() {

	TRISECLR = 0xFF;
	PORTE = 0x0;

}

void user_isr() {
	return;
}
