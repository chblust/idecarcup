#include "Steering.h"
#include "MK64F12.h"
#include "DriveMotor.h"

#define SERVO_FREQ_HZ 50
#define SERVO_SIG_PIN 8
#define FTM3_PRESCALER 125
#define FTM3_MOD_VAL ((DEFAULT_SYSTEM_CLOCK/ FTM3_PRESCALER)/SERVO_FREQ_HZ)

#define LEFT_DC 4.8
#define	RIGHT_DC 8.0

void Steering_init(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;
	
  PORTC_PCR8 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK; //Ch2
	
	// 39.3.10 Disable Write Protection
	FTM3_MODE |= FTM_MODE_WPDIS_MASK;
	
	// 39.3.4 FTM Counter Value
	// Initialize the CNT to 0 before writing to MOD
	FTM3_CNT = 0;
	
	// 39.3.8 Set the Counter Initial Value to 0
	FTM3_CNTIN = 0;
	
	// 39.3.5 Set the Modulo resister
	FTM3_MOD = FTM3_MOD_VAL;

	// 39.3.6 Set the Status and Control of both channels
	// Used to configure mode, edge and level selection
	// See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
	FTM3_C4SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	FTM3_C4SC &= ~FTM_CnSC_ELSA_MASK;
	
	
	
	// 39.3.3 FTM Setup
	// Set prescale value to 1 
	// Chose system clock source
	// Timer Overflow Interrupt Enable
	FTM3_SC = FTM_SC_PS(0x7) |   FTM_SC_CLKS(1); 
}

void set_duty_cycle(float duty_cycle)
{
	FTM3_C4V = (uint16_t) ((((DEFAULT_SYSTEM_CLOCK / 
	FTM3_PRESCALER)/ 
	SERVO_FREQ_HZ) * duty_cycle) / 100);
}

void Steering_set_direction(float dir)
{
	if (dir < 0)
		dir = 0;
	else if (dir > 1)
		dir = 1;
	
	set_duty_cycle(LEFT_DC + (RIGHT_DC - LEFT_DC) * dir);
}