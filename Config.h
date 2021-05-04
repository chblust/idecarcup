#ifndef CONFIG_H
#define CONFIG_H

// set to 0 to disable rear motors (stop them from spinning)
#define ENABLE 1

// pixels that read below this value are assumed to be carpet
#define DARK_THRESHOLD 0.2

// if this many pixels read below DARK_THRESHOLD, carpet detection is triggered
#define CARPET_THRESHOLD 125

// PID constants
#define DEFAULT_KP 0.00675
#define DEFAULT_KI 0.0125
#define DEFAULT_KD -.2 

// per-motor multiplier for max speed fraction when setting both motors
#define MOTOR_CONST 1.5

// enumeration for referring to different modes
#define MODE_AGGRESSIVE 0
#define MODE_PASSIVE_AGGRESSIVE 1
#define MODE_PASSIVE 2

// max motor PWMs associated with different modes
#define AGGRESSIVE_SPEED 70
#define PASSIVE_AGGRESSIVE_SPEED 60
#define PASSIVE_SPEED 45

// how much to add to the brightness diff read from the camera (determined from calibration)
#define CAMERA_CORRECTION_FACTOR 5.0

// anti-drift slowdown is triggered if steering by this much in either direction
#define ANTI_DRIFT_STEERING_THRESHOLD .395

// anti-drift slowdown values per mode
#define AGGRESSIVE_ANTI_DRIFT_MULT 9/10
#define PASSIVE_AGGRESSIVE_ANTI_DRIFT_MULT 1
#define PASSIVE_ANTI_DRIFT_MULT 1

#endif