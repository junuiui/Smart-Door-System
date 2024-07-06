#ifndef DOOR_HANDLER_H
#define DOOR_HANDLER_H

#include <pthread.h>


// only need to worry about these states because
// the motor moves pretty quicky and theres no need to do special actions on the other states
typedef enum {
    DOOR_OPEN = 90,
    DOOR_CLOSED = 0,
} DoorStates;

void door_handler_init(void);
void door_handler_cleanup(void);

int door_handler_get_position();
void door_handler_set_position(int position);

pthread_t door_handler_get_pthread_id();
pthread_mutex_t* door_handler_get_mutex_cleanup_lock();

#endif