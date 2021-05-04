#ifndef DRIVE_MOTOR_H
#define DRIVE_MOTOR_H

#define DEFAULT_SYSTEM_CLOCK    20485760u 
#define DIRECTION_FORWARD 1
#define DIRECTION_REVERSE 0

/**
* Configure FTM timers for controlling PWM output to H-bridges that drive the 
* rear DC drive motors.
*/
void DriveMotor_init(void);

/**
* Write to the H bridge enable pin for motor B, enabling or disabling PWM control.
* enable [in] - 1 or 0, the pin state to write
*/
void DriveMotorB_enable(int enable);

/**
* Write to the H bridge enable pin for motor A, enabling or disabling PWM control.
* enable [in] - 1 or 0, the pin state to write
*/
void DriveMotorA_enable(int enable);

/**
* Change the duty cycle output to the motor B H bridge input,
* in effect changing its speed.
* duty_cycle [in] - 0 to 100
* dir [in] - DIRECTION_FORWARD or DIRECTION_REVERSE as defined above
*/
void DriveMotorB_set_duty_cycle(int duty_cycle, int dir);

/**
* Change the duty cycle output to the motor A H bridge input,
* in effect changing its speed.
* duty_cycle [in] - 0 to 100
* dir [in] - DIRECTION_FORWARD or DIRECTION_REVERSE as defined above
*/
void DriveMotorA_set_duty_cycle(int duty_cycle, int dir);

#endif