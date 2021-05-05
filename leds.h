#ifndef LEDS_H
#define LEDS_H

/**
* Turn the LED off.
*/
void LED_Off(void);

/**
* Configure the GPIO to drive the multicolor LED.
*/
void LED_Init(void);

/**
* Turn on the LED and set its color.
* color [in] - choose from the following:
* y=yellow, r=red, g=green, b=blue, c=cyan, m=magenta, w=white
*/
void LED_On (unsigned char color);

#endif
