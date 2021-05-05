#ifndef TUNING_H
#define TUNING_H

/**
* Set tuned values to compiled defaults.
*/
void Tuning_init(void);

/**
* Receive new tuning values over uart3 if available.
* Node: unused and untested
*/
void Tuning_update(void);

/**
* PID P term constant getter.
*/
float Tuning_get_kp(void);

/**
* PID I term constant getter.
*/
float Tuning_get_ki(void);

/**
* PID D term constant getter.
*/
float Tuning_get_kd(void);

#endif