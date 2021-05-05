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

/**
* [Position detection algorithm]
* Uses latest camera reading.
*
* -divide the camera output into two halves
* -sum the brightness readings for each half
* -subtract the left side sum from the right side sum, return the difference
* 
* Additionally count the number of pixels considered "dark" and return this.
*
* brightness_error [out] - the difference between right side and left side sum
* num_dark [out] - number of pixels considered "dark"
*/
static void measure_camera(float* brightness_error, int* num_dark)
{
	*num_dark = 0;
	float left_sum = 0;
	float right_sum = 0;

	// for each camera pixel...
	for(int i = 0; i < 127; i++)
	{
		float light_level = Camera_get_light_level(i);
	
		if (light_level < DARK_THRESHOLD)
		{
			// count it...
			(*num_dark)++;
			
			// but also make for a dramatic change in the sum of the side
			// This detail is added to make up for smaller differences seen when the track is
			// in darker corners.
			light_level = -1;
		}
		
		// add to the appropriate sum
		if (i < 127/2)
		{
			
			right_sum += light_level;
			
		}
		else
		{
			left_sum += light_level;
		}
	}
	
	// return the brightness error
	*brightness_error = right_sum - left_sum;
}

/**
 * Waits for a delay (in clock cycles*5000)
 * 
 * del - The delay in clock cycles*5000
 *
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
	DriveMotorA_set_duty_cycle(0, DIRECTION_FORWARD);
	DriveMotorB_set_duty_cycle(0, DIRECTION_FORWARD);
	
	Switch2_Init();
	Switch3_Init();
	LED_Init();
	
	static int mode = MODE_AGGRESSIVE;
	
	// Mode Select
	// - switch 3 cycles through modes
	// - switch 2 starts the car in the currently selected mode
	// - leds used to indicate mode:
	// - Green=Passive, Yellow=PassiveAggressive, Red=Aggressive 
	while(!Switch2_Pressed())
	{
		// switch the mode only once
		if (Switch3_Pressed())
		{
			while(Switch3_Pressed());
			// mode is switched on release
			mode = (mode + 1) % 3;
		}
		
		// set LED accordingly
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
	// Switch 2 has been pressed requesting a car start. 
	// Mode is locked in. Determine the max speed for the selected mode.
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
		
		// Determine anti-drift additional slowdown factor based on mode 
    float multiplier;
    if (max_speed == AGGRESSIVE_SPEED) 
		{
      multiplier = AGGRESSIVE_ANTI_DRIFT_MULT;
    }
		else if (max_speed == PASSIVE_AGGRESSIVE_SPEED) 
		{
      multiplier = PASSIVE_AGGRESSIVE_ANTI_DRIFT_MULT;
    } 
		else 
		{
      multiplier = PASSIVE_ANTI_DRIFT_MULT;
    }
	
	// Main control loop
	for(;;)
	{
		// Initiate a new camera capture
		Camera_capture();
		
		// Run the position determination algorithm on the latest camera capture data
		float brightness_diff;
		int num_dark;
		measure_camera(&brightness_diff, &num_dark);
		// Add to the diff to calibrate the value based on our specific camera
		// since our camera reads darker on one side
	  brightness_diff += CAMERA_CORRECTION_FACTOR;
		
		// Carpet Detection
		// If too many pixels were declared to be below a pre-defined threshold, 
		// we determine we're on carpet and stop the vehicle.
		if (num_dark > CARPET_THRESHOLD)
		{
			DriveMotorA_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorB_set_duty_cycle(0, DIRECTION_FORWARD);
			DriveMotorA_enable(0);
			DriveMotorB_enable(0);
			
			// A reset is require to restart
			for(;;);
		}
		
		// Motor Control Algorithm
		
		// enter position detection output into PID loop and get result
		float pid_out =  pid_iterate(0, brightness_diff);
		// map pid output to steering range
		float steering_input = pid_out + .5;
		
		// apply steering control value
		// (steering control is saturated inside this function)
		Steering_set_direction(steering_input);
		
		// saturate pid output to match rear motor control limits
		if (pid_out > .5)
		{
			pid_out = .5;
		}
		else if (pid_out < -.5)
		{
			pid_out = -.5;
		}
		
		// determine duty cycles for rear drive motors based on pid output
		float a_speed = (.5-pid_out) * MOTOR_CONST * max_speed;
    float b_speed = (.5+pid_out) * MOTOR_CONST * max_speed;
    
		// apply anti-drift rear motor slowdown if steering far enough in either direction
    if (fabs(pid_out) > ANTI_DRIFT_STEERING_THRESHOLD) 
		{
			DriveMotorB_set_duty_cycle((b_speed * multiplier), DIRECTION_FORWARD);
      DriveMotorA_set_duty_cycle((a_speed * multiplier), DIRECTION_FORWARD);  
    } 
		else 
		{
      DriveMotorB_set_duty_cycle(b_speed, DIRECTION_FORWARD);
      DriveMotorA_set_duty_cycle(a_speed, DIRECTION_FORWARD);
    }
	}
}