#include "DriveMotor.h"
#include "Steering.h"
#include "Camera.h"
#include "uart.h"
#include <stdio.h>
#include <math.h>

#define ENABLE 1
#define SPEED 30

#define DARK_THRESHOLD 0.2
#define CARPET_THRESHOLD 120
#define STEERING_MULTIPLIER .0035
#define STEERING_START 0.4

#define REQUIRED_DIFF 5
static float I = STEERING_START;
static float compute_direction(void)
{
	
	int num_dark = 0;
	// first check for carpet
		for(int i = 0; i < 127; i++)
		{
			if (Camera_get_light_level(i) < DARK_THRESHOLD)
			{
				num_dark++;
			}
		}
		
		if (num_dark > CARPET_THRESHOLD)
		{
			DriveMotorA_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorB_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorA_enable(0);
			DriveMotorB_enable(0);
		}
		
		// sum pixels on each side of camera
		float left_sum = 0;
		float right_sum = 0;
		for (int i = 0; i < 127; i++)
		{
			if (i > 127/2)
			{
				left_sum += Camera_get_light_level(i);
			}
			else
			{
				right_sum += Camera_get_light_level(i);
			}
		}
		float steering_control;
		float steering_diff = left_sum - right_sum;
		if (fabs(steering_diff) >= REQUIRED_DIFF)
		{
			steering_control = (steering_diff) * STEERING_MULTIPLIER;
		}
		else
		{
			steering_control = 0;
			I = STEERING_START;
		}
		float retval = I + steering_control;
		I += steering_control;
		
		if (I > 1) I = 1;
		else if (I < 0) I = 0;
		return retval;
}

/**
 * Waits for a delay (in milliseconds)
 * 
 * del - The delay in milliseconds
 */
void delay(int del){
	volatile int i;
	for (i=0; i<del*5000; i++){
		// Do nothing
	}
}

int main(void)
{
	uart0_init();
	uart0_put("Waddup\r\n");
	DriveMotor_init();
	Steering_init();
	Camera_init();
	Steering_set_direction(0.4);
	Camera_capture();
	// initial delay
	delay(20);
	
	DriveMotorA_enable(ENABLE);
	DriveMotorB_enable(ENABLE);
	DriveMotorA_set_duty_cycle(SPEED, DIRECTION_FORWARD);
	DriveMotorB_set_duty_cycle(SPEED, DIRECTION_FORWARD);
	//for(;;);
	for(;;)
	{
		Camera_capture();
		
		// figure out what direction should be pointing
		
		Steering_set_direction(compute_direction());
		// input new direction into PI loop
		
		// use PI loop output to control steering
		
		// delay reasonably
		//	delay(1);
		
                //uart0_put(str);
		/*char str[100];
                for (int i = 0; i < 128; i++) {
                    sprintf(str,"%.2f ", Camera_get_light_level(i));
                    uart0_put(str);
                }
								uart0_put("\r\n");*/
								//delay(10);
       
		
		
	}
}