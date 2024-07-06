#include "hal/joystick.h"
#include "../../app/include/direction.h"
#include "../../app/include/utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

static bool is_initialized = false;

void joystick_init(void) {

    printf("joystick - initializing\n");
    assert(!is_initialized);
    is_initialized = true;

    // run_command("config-pin p8.14 gpio");
    // run_command("config-pin p8.15 gpio");
    // run_command("config-pin p8.16 gpio");
    // run_command("config-pin p8.17 gpio");
    // run_command("config-pin p8.18 gpio");


    // file_write(JS_BASE_PATH JS_UP_PATH JS_DIRECTION_PATH, "in");
    // file_write(JS_BASE_PATH JS_RIGHT_PATH JS_DIRECTION_PATH, "in");
    // file_write(JS_BASE_PATH JS_DOWN_PATH JS_DIRECTION_PATH, "in");
    // file_write(JS_BASE_PATH JS_LEFT_PATH JS_DIRECTION_PATH, "in");
    // file_write(JS_BASE_PATH JS_PUSH_PATH JS_DIRECTION_PATH, "in");
}

void joystick_cleanup(void) {

    printf("joystick - cleanup\n");
    assert(is_initialized);
    is_initialized = false;
}


Direction joystick_get_input(){

    char* up_value = file_read(1, JS_BASE_PATH JS_UP_PATH JS_VALUE_PATH);
    char* right_value = file_read(1, JS_BASE_PATH JS_RIGHT_PATH JS_VALUE_PATH);
    char* down_value = file_read(1, JS_BASE_PATH JS_DOWN_PATH JS_VALUE_PATH);
    char* left_value = file_read(1, JS_BASE_PATH JS_LEFT_PATH JS_VALUE_PATH);
    char* push_value = file_read(1, JS_BASE_PATH JS_PUSH_PATH JS_VALUE_PATH);

    int up_pressed = strcmp(up_value, "0");
    int right_pressed = strcmp(right_value, "0");
    int down_pressed = strcmp(down_value, "0");
    int left_pressed = strcmp(left_value, "0");
    int push_pressed = strcmp(push_value, "0");

    free(up_value);
    free(right_value);
    free(down_value);
    free(left_value);
    free(push_value);

    up_value = right_value = down_value = left_value = push_value = NULL;

    if(up_pressed == 0) return UP;
    if(right_pressed == 0) return RIGHT;
    if(down_pressed == 0) return DOWN;
    if(left_pressed == 0) return LEFT;
    if(push_pressed == 0) return PUSH;
    
    return NO_DIRECTION;
}