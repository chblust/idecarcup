#ifndef TUNING_H
#define TUNING_H

void Tuning_init(void);

void Tuning_update(void);

float Tuning_get_kp(void);
float Tuning_get_ki(void);
float Tuning_get_kd(void);

#endif