#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "hal/servo.h"
#include "../../app/include/utils.h"

static bool is_initialized = false;

void servo_init(void) {

    printf("servo - initializing\n");
    assert(!is_initialized);
    is_initialized = true;

    run_command("config-pin p9.21 pwm");
    file_write(SERVO_PWM "enable", "1");

    // 50 hz communication (2ns)
    // start at 0 degrees
    file_write(SERVO_PWM "period",      "20000000");
    // servo_set_position_deg(0);
}

void servo_cleanup(void) {
    // Free any memory, close files, ...
    printf("servo - cleanup\n");
    assert(is_initialized);
    is_initialized = false;

    //reset to 0
    // servo_set_position_deg(0);

    sleep(1);
    file_write(SERVO_PWM "enable", "0");
}

// only the duty_cycle needs to be changed
// period (50hz) will stay the same the whole time
bool servo_set_position_deg(int degrees) {

    if (degrees > 90 || degrees < -90) return false;

    // 1ms of duty_cycle translates to ~90 degrees
    double degree_ns_ratio = 1000000 / 90;

    // duty cycle range 0.5ms to 2.5ms corresponding to -90 to 90 degrees respectively
    int duty_ns = (1500000) + (double) degrees * degree_ns_ratio;

    // only need to worry about numbers below 100 (degrees)
    char str_duty[8];

    sprintf(str_duty, "%d", duty_ns);
    file_write(SERVO_PWM "duty_cycle", str_duty);

    return true;
}