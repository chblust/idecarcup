#include "DriveMotor.h"
#include "Steering.h"
#include "Camera.h"
#include "uart.h"
#include <stdio.h>
#include <math.h>
#include "Config.h"
#include "pid.h"
#include "Tuning.h"
#include "switch.h"
#include "leds.h"

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
	
	*brightness_error = right_sum - left_sum;
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
	//Tuning_init();
	
	
	DriveMotor_init();
	Steering_init();
	Camera_init();
	Steering_set_direction(0.4);
	Camera_capture();
	// initial delay
	delay(20);
	
	DriveMotorA_enable(ENABLE);
	DriveMotorB_enable(ENABLE);
	DriveMotorA_set_duty_cycle(0, DIRECTION_FORWARD);
	DriveMotorB_set_duty_cycle(0, DIRECTION_FORWARD);
	
	Switch2_Init();
	Switch3_Init();
	LED_Init();
	
	static int mode = MODE_AGGRESSIVE;
	
	// mode select
	while(!Switch2_Pressed())
	{
		if (Switch3_Pressed())
		{
			while(Switch3_Pressed());
			mode = (mode + 1) % 3;
		}
		
		if (mode == MODE_AGGRESSIVE)
		{
			LED_On('r');
		}
		else if (mode == MODE_PASSIVE_AGGRESSIVE)
		{
			LED_On('y');
		}
		else 
		{
			LED_On('g');
		}
	}
	
	//for(;;);
	for(;;)
	{
		Camera_capture();
		
		// figure out what direction should be pointing
		float brightness_diff;
		int num_dark;
		measure_camera(&brightness_diff, &num_dark);
	  brightness_diff += 5.0;

		//Tuning_update();
		//char buf[50];
		//sprintf(buf, "right-left: %f\r\n", brightness_diff);
		//uart0_put(buf);
		
		// Carpet Detection
		// Stop car if on carpet
		if (num_dark > CARPET_THRESHOLD)
		{
			DriveMotorA_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorB_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorA_enable(0);
			DriveMotorB_enable(0);
			for(;;);
		}
		
		//if (brightness_diff < REQUIRED_DIFF)
		//{
		//	brightness_diff = 0;
		//}
		
		// enter error into PID loop and get result
		float pid_out =  pid_iterate(0, brightness_diff);
		float steering_input = pid_out + .5;
		
		//if (num_dark < 25)
		//{
		//steering_input = 0.5;
	//	}
		
		// (steering control is saturated inside this function)
		Steering_set_direction(steering_input);
		if (pid_out > .5)
		{
			pid_out = .5;
		}
		else if (pid_out < -.5)
		{
			pid_out = -.5;
		}
		//DriveMotorB_set_duty_cycle(0,1);
		//DriveMotorA_set_duty_cycle(0,1);
		//char str[50];
		//sprintf(str, "PID: %f\n\r", pid_out);
		//uart0_put(str);
		float max_speed = 0;
		
		if (mode == MODE_AGGRESSIVE)
		{
				max_speed = AGGRESSIVE_SPEED;
		}
		else if (mode == MODE_PASSIVE_AGGRESSIVE)
		{
				max_speed = PASSIVE_AGGRESSIVE_SPEED;
		}
		else
		{
				max_speed = PASSIVE_SPEED;
		}
		
		DriveMotorB_set_duty_cycle((.5+pid_out) * MOTOR_CONST * max_speed, DIRECTION_FORWARD);
		DriveMotorA_set_duty_cycle((.5-pid_out)*MOTOR_CONST * max_speed, DIRECTION_FORWARD);
	}
}