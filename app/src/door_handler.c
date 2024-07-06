
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "display_handler.h"
#include "door_handler.h"
#include "utils.h"
#include "../../hal/include/hal/buzzer.h"
#include "../../hal/include/hal/joystick.h"
#include "../../hal/include/hal/motion_sensor.h"
#include "../../hal/include/hal/neopixel.h"
#include "../../hal/include/hal/servo.h"

static pthread_t thread_id;

static bool is_initialized = false;
static bool cancel_thread = false;
_Atomic int door_position;

pthread_mutex_t mutex_cleanup;
pthread_mutex_t mutex_target;

pthread_t door_handler_get_pthread_id() {
    return thread_id;
}

pthread_mutex_t* door_handler_get_mutex_cleanup_lock(){
    return &mutex_cleanup;
}

int door_handler_get_position(){
    return door_position;
}

void door_handler_set_position(int position) {
    door_position = position;
}

static void* door_handler_start(){

    while(!cancel_thread){ 


        //check for rfid or motion to trigger 'door' open and close
        if(motion_sensor_get_distance() <= 20){

            if(door_position == DOOR_CLOSED){

                printf("door_handler - opening door\n");
                // neopixel_is_door_locked(1);
                
                servo_set_position_deg(DOOR_OPEN);
                door_position = DOOR_OPEN;

                display_handler_update_value(8 * 1000);

                // busy wait
                while(display_handler_get_value() > 0){
                    // printf("distance : %.2f\n", motion_sensor_get_distance());

                    // Check for exit case
                    if (joystick_get_input() == RIGHT){
                        cancel_thread = true;
                        display_handler_update_value(0);
                        break;
                    }
                }

                printf("door_handler - automatically closing door\n");

                // add buzzer sound
                servo_set_position_deg(DOOR_CLOSED);
                door_position = DOOR_CLOSED;
            }
        }

        //handle quit here
        if(joystick_get_input() == RIGHT){
            cancel_thread = true;
        }
        
    }

    pthread_mutex_unlock(&mutex_cleanup);
    return NULL;
}

void door_handler_init(void){

    printf("door_handler - init\n");
    assert(!is_initialized);
    is_initialized = true;

    srand(time(NULL));

    pthread_mutex_init(&mutex_cleanup, NULL);
    pthread_mutex_lock(&mutex_cleanup);

    servo_set_position_deg(DOOR_CLOSED);
    door_position = DOOR_CLOSED;
    pthread_create(&thread_id, NULL, door_handler_start, NULL);

}
void door_handler_cleanup(void){

    printf("door_handler - cleanup\n");
    is_initialized = false;
}