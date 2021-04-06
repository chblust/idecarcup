#ifndef DRIVE_MOTOR_H
#define DRIVE_MOTOR_H

#define DEFAULT_SYSTEM_CLOCK    20485760u 
#define DIRECTION_FORWARD 1
#define DIRECTION_REVERSE 0

void DriveMotor_init(void);

void DriveMotorB_enable(int enable);

void DriveMotorA_enable(int enable);

void DriveMotorB_set_duty_cycle(int duty_cycle, int dir);

void DriveMotorA_set_duty_cycle(int duty_cycle, int dir);

#endif