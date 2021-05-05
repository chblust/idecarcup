#include "MK64F12.h"
#include "DriveMotor.h"

#define FTM0_FREQ_HZ 10000
#define FTM0_MOD_VALUE          (DEFAULT_SYSTEM_CLOCK/FTM0_FREQ_HZ)

#define MOTOR_A_ENABLE_PIN 2 
#define MOTOR_B_ENABLE_PIN 3

void DriveMotor_init(void)
{
		// don't start with the motor spinning
		DriveMotorA_enable(0);
		DriveMotorB_enable(0);
	
		// set up relevant pins for both motors
		// turn on both port B and C
		SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTB_MASK;
	
		// configure port B
		PORTB_PCR2 = PORT_PCR_MUX(1);
		PORTB_PCR3 = PORT_PCR_MUX(1);
		GPIOB_PDDR |= (1 << MOTOR_A_ENABLE_PIN) | (1 << MOTOR_B_ENABLE_PIN);
	
		// 12.2.13 Enable the clock to the FTM0 Module
		SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	
	
	
		// 11.4.1 Route the output of FTM channels 0-3 to the pins
		// Use drive strength enable flag to high drive strength
    PORTC_PCR3 = PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK; //Ch2
    PORTC_PCR4 = PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK; //Ch3
		PORTC_PCR1 = PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK; //Ch2
    PORTC_PCR2 = PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK; //Ch3
	
		// 39.3.10 Disable Write Protection
		FTM0_MODE |= FTM_MODE_WPDIS_MASK;
	
		// 39.3.4 FTM Counter Value
		// Initialize the CNT to 0 before writing to MOD
		FTM0_CNT = 0;
	
		// 39.3.8 Set the Counter Initial Value to 0
		FTM0_CNTIN = 0;
		
		// 39.3.5 Set the Modulo resister
		FTM0_MOD = FTM0_MOD_VALUE;

		// 39.3.6 Set the Status and Control of both channels
		// Used to configure mode, edge and level selection
		// See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
		FTM0_C3SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
		FTM0_C3SC &= ~FTM_CnSC_ELSA_MASK;
		
		// See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
		FTM0_C2SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
		FTM0_C2SC &= ~FTM_CnSC_ELSA_MASK;
	
		// See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
		FTM0_C1SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
		FTM0_C1SC &= ~FTM_CnSC_ELSA_MASK;
	
		// See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
		FTM0_C0SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
		FTM0_C0SC &= ~FTM_CnSC_ELSA_MASK;
	
		// 39.3.3 FTM Setup
		// Set prescale value to 1 
		// Chose system clock source
		// Timer Overflow Interrupt Enable
		FTM0_SC = FTM_SC_PS(0) | FTM_SC_CLKS(1); 
}

void DriveMotorB_enable(int enable)
{
		// saturate enable to 0 or 1
		enable = enable & 1;
		
		GPIOB_PDOR |= (enable << MOTOR_B_ENABLE_PIN);
}

void DriveMotorA_enable(int enable)
{
		// saturate enable to 0 or 1
		enable = enable & 1;
		GPIOB_PDOR |= (enable << MOTOR_A_ENABLE_PIN);
}

void DriveMotorB_set_duty_cycle(int duty_cycle, int dir)
{
		// Calculate the new cutoff value
		uint16_t mod = (uint16_t) (((DEFAULT_SYSTEM_CLOCK / FTM0_FREQ_HZ) * duty_cycle) / 100);
		
		// From direction, set channel outputs 
		if(dir) {
				FTM0_C3V = mod; 
				FTM0_C2V = 0;
		} else {
				FTM0_C2V = mod; 
				FTM0_C3V = 0;
		}

		// Write to the clock again to reset the modulus
		FTM0_MOD = (DEFAULT_SYSTEM_CLOCK / FTM0_FREQ_HZ);
}

void DriveMotorA_set_duty_cycle(int duty_cycle, int dir)
{
		// Calculate the new cutoff value
		uint16_t mod = (uint16_t) (((DEFAULT_SYSTEM_CLOCK / FTM0_FREQ_HZ) * duty_cycle) / 100);
  
		// From direction, set channel outputs 
		if(dir) {
				FTM0_C0V = mod; 
				FTM0_C1V = 0;
		} else {
				FTM0_C1V = mod; 
				FTM0_C0V = 0;
		}

		// Write to the clock again to reset the modulus
		FTM0_MOD = (DEFAULT_SYSTEM_CLOCK / FTM0_FREQ_HZ);
}