/*
File that contains the main function.
Written by Dag Hjelm and Joar Rutqvist 2017.
*/
#include <stdint.h>
#include <pic32mx.h>
#include "header.h"

uint8_t led[128*4];

int main(void) {
	configbtn();
	configLed();
	configscreen();
	configtimer();
	display_init();
	render(led);
//	display_image(10, icon);
//	display_image(50, icon);
	PORTE = 1;
    run();


}
