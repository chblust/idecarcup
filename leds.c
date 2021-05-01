#include "leds.h"
#include "MK64F12.h"
#define RED_PIN 22
#define BLUE_PIN 21
#define GREEN_PIN 26

void LED_Off(void)
{
	// set the bits to ONE to turn off LEDs
	// use PSOR to set a bit
	GPIOB_PSOR = (1 << RED_PIN) | (1 << BLUE_PIN);
	GPIOE_PSOR = (1 << GREEN_PIN);
}

void LED_Init(void)
{
	// Enable clocks on Ports B and E for LED timing
	SIM_SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK);
	// We use PortB for RED and BLUE LED
	// RED PTB22, port B pin 22
	// BLUE PTB 21, port B pin 21
	// GREEN PTE 26, port E pin 26
	// We use PortE for GREEN LED
	// 12.2.12 System Clock Gating Control Register 5
	// Port B is Bit 10
	// Port E is Bit 13
				   // 0x0400 (Bit 10)                 0x2000 (Bit 13)
	 
	// Configure the Signal Multiplexer for GPIO
 	// Pin Control Register n  
	PORTB_PCR22 = PORT_PCR_MUX(1);
	PORTB_PCR21 = PORT_PCR_MUX(1);
	PORTE_PCR26 = PORT_PCR_MUX(1);
	

	// Switch the GPIO pins to output mode
	// GPIOB_DDR is the direction control for Port B
	// GPIOE_DDR is the direction control for Port E
	// set it to ONE at BIT21, 22 on Port B for an output
	// set it to ONE at Bit26 on Port E for an output	 
GPIOB_PDDR |= (1 << RED_PIN) | (1 << BLUE_PIN);
GPIOE_PDDR |= (1 << GREEN_PIN);

	// Turn off the LEDs
    LED_Off();
		//GPIOE_PCOR |= (1 << GREEN_PIN);
}

static void red_led(int state)
{
	if (state)
	{
		GPIOB_PCOR = (1 << RED_PIN);
	}
	else
	{
		GPIOB_PSOR = (1 << RED_PIN);
	}
}

static void blue_led(int state)
{
	if (state)
	{
		GPIOB_PCOR = (1 << BLUE_PIN);
	}
	else
	{
		GPIOB_PSOR = (1 << BLUE_PIN);
	}
}

static void green_led(int state)
{
	if (state)
	{
		GPIOE_PCOR = (1 << GREEN_PIN);
	}
	else
	{
		GPIOE_PSOR = (1 << GREEN_PIN);
	}
}

void LED_On (unsigned char color)
{
	// set the appropriate color
	// you need to drive the appropriate pin OFF to turn on the LED
	switch(color)
	{
		case 'y':
			red_led(1);
			green_led(1);
			blue_led(0);
			break;
		case 'r':
			red_led(1);
			green_led(0);
			blue_led(0);
			break;
		case 'g':
			red_led(0);
			green_led(1);
			blue_led(0);
			break;
		case 'b':
			red_led(0);
			green_led(0);
			blue_led(1);
			break;
		case 'c':
			red_led(0);
			green_led(1);
			blue_led(1);
			break;
		case 'm':
			red_led(1);
			green_led(0);
			blue_led(1);
			break;
		case 'w':
			red_led(1);
			green_led(1);
			blue_led(1);
			break;
		default:
			break;
	}
}
