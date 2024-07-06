#include "hal/buzzer.h"
#include "../../app/include/utils.h"
#include <stdio.h>     // Include this for standard I/O functions
#include <stdlib.h>
#include <time.h>

// defines
#define PWM "config-pin p9_22 pwm"
#define BUZZER_PERIOD "/dev/bone/pwm/0/a/period"
#define BUZZER_DUTY_CYCLE "/dev/bone/pwm/0/a/duty_cycle"
#define BUZZER_ENABLE "/dev/bone/pwm/0/a/enable"

// functions
void buzzer_init(){
    // Configure the necessary pins as PWM for ZenCape Buzzer
    run_command(PWM);
}

void buzzer_cleanup(){
    printf("Cleaning Buzzer........");

    buzzer_off();

    printf("  DONE!\n");
}

void buzzer_on(){
    file_write(BUZZER_ENABLE, "1");
}

void buzzer_off(){
    file_write(BUZZER_ENABLE, "0");
}

void buzzer_verify(int flag){
    int count = 0;

    // Fail
    if (flag == 0){
        while (count < 4){
            
            buzzer_playSound(100, 100);

            count++;
        }
    }
    // Success
    else if (flag == 1){
        while (count < 1){
            
            buzzer_playSound(600, 0);

            count++;
        }
    }
    else{
        printf("Error: Invalid value provided for flag. Value must be either 0 or 1. Current value: %d\n", flag);
        return; // return nothing 
    }
}

void buzzer_playSound(long long playTimeMS, long long stopTimeMS){
    // Set Period and Cycle
    file_write(BUZZER_PERIOD, "1000000");
    file_write(BUZZER_DUTY_CYCLE, "500000");

    // Play the sound for playTimeMS
    buzzer_on();
    sleep_for_ms(playTimeMS);

    // Stop the sound for stopTimeMS
    buzzer_off();
    sleep_for_ms(stopTimeMS);
}