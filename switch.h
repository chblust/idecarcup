#ifndef SWITCH_H
#define SWITCH_H
	
/**
* Configure GPIO for reading switch 2 events.
*/
void Switch2_Init(void);

/**
* Read if switch2 is currently pressed.
* return 1 if pressed, 0 otherwise
*/
unsigned char Switch2_Pressed(void);

/**
* Configure GPIO for reading switch 3 events.
*/
void Switch3_Init(void);

/**
* Read if switch3 is currently pressed.
* return 1 if pressed, 0 otherwise
*/
unsigned char Switch3_Pressed(void);

#endif
