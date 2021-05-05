#include "pid.h"
#include "Config.h"
#include "Tuning.h"

// running values for each PID component that sum (with their coefficients) to the PID output
float P, I, D;

float pid_iterate(float target, float actual)
{
	// tells the loop if its the first iteration or not
	static float initialized = 0;
	
	static float last_error = 0;
	
	// get current error
	float error = target - actual;
	
	// calculate P term
	P = error * Tuning_get_kp();
	
	float output;
	if (initialized)
	{
		// calculate I and D terms
		I = (last_error + error) * Tuning_get_ki();
		D = (error - last_error) * Tuning_get_kd();
		
		// sum terms to get output
		output = P + I + D;
	}
	else
	{
		// cant calculate I and D terms yet
		output = P;
	}
	
	// remember last error for calculating I and D terms in the future
	last_error = error;
	return output;
}



