#ifndef _BUZZER_H
#define _BUZZER_H

/**
 * Initialize Buzzer
 * Congifure the P9.22 pin to PWM 
*/
void buzzer_init(void);

/**
 * Cleaning up the Buzzer 
 *      1. Turning off the Buzzer
*/
void buzzer_cleanup(void);

/**
 * Turning the Buzzer on (Unmute)
*/
void buzzer_on(void);

/**
 * Turning the Buzzer off (Mute)
*/
void buzzer_off(void);

/**
 * Play sound if...
 *      flag (= 0)  --> Fail 
 *      flag (= 1)  --> Success
*/
void buzzer_verify(int);

/**
 * Play sound 
 * @param playTimeMS play duration of sound in milliseconds
 * @param stopTimeMS stop duration of sound in milliseconds
*/
void buzzer_playSound(long long, long long);

#endif