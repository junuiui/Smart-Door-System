/* NEOPIXEL MODULE
- colors array added in shared_data_struct
- functions provided by prof added to pru/main.c
- modified this module to use the shared_data_struct and the functions provided by prof
- added neopixel functions to main 
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

#include "hal/neopixel.h"
#include "../../app/include/utils.h"
#include "../../app/include/pru.h"
#include "../../app/include/shared_data_struct.h"

#define PIN "config-pin p8_11 pruout"
#define NUM_LED 8
//colors
#define OFF 0x00000000
#define DIM_RED 0x000f0000
#define DIM_GREEN 0x0f000000
#define DIM_BLUE 0x00000f00
#define WHITE 0x0f0f0f00
#define BRIGHT_RED 0x00f00000
#define BRIGHT_GREEN 0xf0000000
#define BRIGHT_BLUE 0x0000f000
#define AMBER 0x20f00000

volatile void* pru_base_neo;
volatile shared_data_struct* shared_data_neo;

void neopixel_init(void) {
    printf("neopixel - initializing\n");
    run_command(PIN);
    pru_base_neo = get_pru_mmap_addr();
    shared_data_neo = PRU0_MEM_FROM_BASE(pru_base_neo);

    neopixel_set_all_leds(0);
}

void neopixel_cleanup(void) {
    printf("neopixel - cleanup\n");
    neopixel_set_all_leds(OFF);
}

void neopixel_change_led_color(uint32_t color, int position){
    shared_data_neo->colors[position] = color;
}

uint32_t set_led_color(int r, int g, int b) {
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255){
        printf("Error: Invalid value provided for color. Values must be between 0 and 255. Current values: r=%d, g=%d, b=%d\n", r, g, b);
        return 0x00000000;
    }

    uint32_t col = ((uint32_t)g << 24) | ((uint32_t)r << 16) | ((uint32_t)b << 8);
    return col;
}

void neopixel_set_all_leds(uint32_t color){
    for(int i = 0; i <= NUM_LED; i++){
        shared_data_neo->colors[i] = color;
    }
}

void neopixel_unlocked(void){
    //make amber light flash 2 times 
    for(int i = 0; i < 2; i++){
        neopixel_set_all_leds(AMBER);
        sleep_for_ms(200);
        neopixel_set_all_leds(OFF);
        sleep_for_ms(150);
    }
    sleep_for_ms(200);
    neopixel_set_all_leds(DIM_GREEN);
}

void neopixel_locked(void){
    //make amber light flash 3 times 
    for(int i = 0; i < 3; i++){
        neopixel_set_all_leds(AMBER);
        sleep_for_ms(200);
        neopixel_set_all_leds(OFF);
        sleep_for_ms(150);
    }
    sleep_for_ms(200);
    neopixel_set_all_leds(DIM_RED);
}

void neopixel_is_door_locked(int flag){

    // Fail
    if (flag == 0){
        printf("Door is Locked\n");
        neopixel_locked();
    }
    // Success
    else if (flag == 1){  
        printf("Door is Unlocked\n");
        neopixel_unlocked();
    }
    else{
        neopixel_set_all_leds(OFF);
        printf("Error: Invalid value provided for flag. Value must be either 0 or 1. Current value: %d\n", flag);
        return; // return nothing 
    }
}
