#ifndef PID_H
#define PID_H

/**
* Run an iteration of a PID loop and get the output.
* -loop is persistance and incorporates I and D terms after first input.
* -uses constants from Tuning module
* return floating point corrective output, can be negative
*/
float pid_iterate(float target, float actual);

#endif