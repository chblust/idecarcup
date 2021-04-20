#include "DriveMotor.h"
#include "Steering.h"
#include "Camera.h"
#include "uart.h"
#include <stdio.h>
#include <math.h>
#include "Config.h"
#include "pid.h"
#include "Tuning.h"

static void measure_camera(float* brightness_error, int* num_dark)
{
	*num_dark = 0;
	float left_sum = 0;
	float right_sum = 0;

	for(int i = 0; i < 127; i++)
	{
		float light_level = Camera_get_light_level(i);
		if (light_level < DARK_THRESHOLD)
		{
			(*num_dark)++;
		}
		
		if (i < 127/2)
		{
			right_sum += light_level;
		}
		else
		{
			left_sum += light_level;
		}
	}
	
	*brightness_error = left_sum - right_sum;
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
	Tuning_init();
	
	
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
		float brightness_diff;
		int num_dark;
		measure_camera(&brightness_diff, &num_dark);
		Tuning_update();
		
		// Carpet Detection
		// Stop car if on carpet
		if (num_dark > CARPET_THRESHOLD)
		{
			DriveMotorA_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorB_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorA_enable(0);
			DriveMotorB_enable(0);
		}
		
		// enter error into PID loop and get result
		float steering_input = pid_iterate(0, brightness_diff);
		
		// (steering control is saturated inside this function)
		Steering_set_direction(steering_input);
	}
}