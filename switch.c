/* 
Title:  Switch
Purpose:   Perform the actions specified in the Lab Manual for Lab01    
Name:   Andrew Simmons
Date:   01/29/2021
*/

// NOTE: The LEDs and the Switches on the K64 are all active LOW

#include "MK64F12.h"                    // Device header
#include "switch.h"


void Switch2_Init(void)
{
    // 12.2.12 System Clock Gating Control Register 5
    // Port C is Bit 11 in SCGC5
    // Enable clock for Port C PTC6 button
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
    
    // Configure the Mux for the button
		PORTC_PCR6 &= ~(PORT_PCR_MUX_MASK);
    PORTC_PCR6 |= PORT_PCR_MUX(1);
    
    // Configure the interrupt to allow on rising and falling edge
    PORTC_PCR6 |= PORT_PCR_IRQC(0b1011);

    // Set the push button as an input
    // GPIOC_DDR is the direction control for Port C
    // set it to ZERO at BIT6 for an input
    
    GPIOC_PDDR |= ~(1u << 6);

    //NVIC_EnableIRQ(PORTC_IRQn);
}

void Switch3_Init(void)
{
    // 12.2.12 System Clock Gating Control Register 5
    // Port A is Bit 9 in SCGC5
    // Enable clock for Port A PTA4 button
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
    
    // Configure the Mux for the button
    PORTA_PCR4 &= ~(PORT_PCR_MUX_MASK);
    PORTA_PCR4 |= PORT_PCR_MUX(1);

    // Configure the interrupt to only allow on rising edge
    PORTA_PCR4 |= PORT_PCR_IRQC(0b1001);
    PORTA_PCR4 |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    
    // Set the push button as an input
    // GPIOA_DDR is the direction control for Port A
    // set it to ZERO at BIT4 for an input
    
    GPIOA_PDDR &= ~(1u << 4);

    //NVIC_EnableIRQ(PORTA_IRQn);
}

unsigned char Switch2_Pressed(void)
{
    // check if switch2 pressed ()
    // if PORT C - BIT6 is ZERO, then the switch is pressed
    // PDIR - Port Data Input Register
    // return a ZERO if NOT Pressed
    // return a 0xFF if Pressed

    if ((GPIOC_PDIR & (1u << 6)) == 0) {
        return 0xFF;
    } else {
        return 0;
    }
}

unsigned char Switch3_Pressed(void)
{
    // check if switch3 pressed ()
    // if PORT A - BIT4 is ZERO, then the switch is pressed
    // PDIR - Port Data Input Register
    // return a ZERO if NOT Pressed
    // return a 0xFF if Pressed

    if ((GPIOA_PDIR & (1u << 4)) == 0) {
        return 0xFF;
    } else {
        return 0;
    }
}
