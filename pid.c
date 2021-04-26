#include "pid.h"
#include "Config.h"
#include "Tuning.h"

float last_error;

float P, I, D;

float pid_iterate(float target, float actual)
{
	// tells the loop if its the first iteration or not
	static float initialized = 0;
	
	static float last_error = 0;
	float error = target - actual;
	
	P = error * Tuning_get_kp();
	float output;

	if (initialized)
	{
		I = (last_error + error) * Tuning_get_ki();
		D = (error - last_error) * Tuning_get_kd();
		output = P + I + D;
	}
	else
	{
		output = P;
	}
	
	last_error = error;
	return output;
}



