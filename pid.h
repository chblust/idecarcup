#ifndef PID_H
#define PID_H

void pid_set_kp(float new_kp);
void pid_set_ki(float new_ki);
void pid_set_kd(float new_kd);

float pid_iterate(float target, float actual);

#endif