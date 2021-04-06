#ifndef CAMERA_H
#define CAMERA_H

void Camera_init(void);

/**
* Gets the latest value of the pixel at the address provided.
* addr [in] - address of pixel, 0-127
* return 0-1
**/
float Camera_get_light_level(unsigned int addr);

void Camera_capture(void);

#endif