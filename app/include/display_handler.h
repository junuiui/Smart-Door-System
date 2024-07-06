#ifndef _DISPLAY_CONTROLLER_
#define _DISPLAY_CONTROLLER_

#include <pthread.h>

void display_handler_init();
void display_handler_cleanup(void);

int display_handler_get_value();
void display_handler_update_value(int new_value);

pthread_t display_handler_get_pthread_id();

#endif