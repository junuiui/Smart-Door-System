// Sample button module 
// Part of the Hardware Abstraction Layer (HAL) 

#ifndef _SERVO_H_
#define _SERVO_H_

#include <stdbool.h>

#define SERVO_PWM "/dev/bone/pwm/0/b/" // P9-21
#define SERVO_PWM2 "/dev/bone/pwm/2/b" // P8-13

void servo_init(void);
void servo_cleanup(void);

// ~180 degree of movement
// starts at '0' and can move 90 degrees each direction
bool servo_set_position_deg(int);

#endif