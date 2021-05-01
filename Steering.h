#ifndef STEERING_H
#define STEERING_H

void Steering_init(void);

/**
* 0 = left
* 0.5 = straight
* 1 = right
**/
void Steering_set_direction(float dir);

#endif