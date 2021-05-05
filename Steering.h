#ifndef STEERING_H
#define STEERING_H

/**
* Configure FTM timer for servo output.
*/
void Steering_init(void);

/**
* Set the servo motor to a new position, according to the following:
* dir = 0 => left
* dir = 0.5 => straight
* dir = 1 => right
* (values between the above thresholds correspond to the angles in between)
**/
void Steering_set_direction(float dir);

#endif