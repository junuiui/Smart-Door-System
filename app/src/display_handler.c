#include <stdlib.h> 
#include <stdio.h> 
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "../include/utils.h"
#include "../include/pru.h"
#include "../include/display_handler.h"
#include "../include/shared_data_struct.h"
#include "../../hal/include/hal/display.h"

static bool is_initialized = false;
static bool cancel_thread = false;

static pthread_t thread_id;

volatile void* pru_base;
volatile shared_data_struct* shared_data;

pthread_t display_handler_get_pthread_id(){
    return thread_id;
}

static pthread_mutex_t mutex;

void display_handler_update_value(int new_value){

    pthread_mutex_lock(&mutex);
    shared_data->value_ms = new_value;
    pthread_mutex_unlock(&mutex);
}

int display_handler_get_value(){

    pthread_mutex_lock(&mutex);
    int result = shared_data->value_ms;
    pthread_mutex_unlock(&mutex);
    
    return result;
}


static void* display_handler_start(){

    pru_base = get_pru_mmap_addr();
    shared_data = PRU0_MEM_FROM_BASE(pru_base);


    while (!cancel_thread){
        display_value((shared_data->value_ms / 100));

    }
    return NULL;
}

void display_handler_init(){
 
    printf("display_handler - initializing\n");
    assert(!is_initialized);
    is_initialized = true;

    // Check PRU is running
    char *pru_state = file_read(256, "/sys/class/remoteproc/remoteproc1/state");
    if (strcmp(pru_state, "running\n") != 0){
        printf("ERROR display_handler_init() : PRU is not running!\n");
        return;
    }

    pthread_create(&thread_id, NULL, display_handler_start, NULL);
    
}

void display_handler_cleanup(){

    is_initialized = false;
    cancel_thread = true;
    printf("display_handler - cleanup\n");

}