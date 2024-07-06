#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#define JS_BASE_PATH "/sys/class/gpio/"
#define JS_UP_PATH "gpio26/"
#define JS_RIGHT_PATH "gpio47/"
#define JS_DOWN_PATH "gpio46/"
#define JS_LEFT_PATH "gpio65/"
#define JS_PUSH_PATH "gpio27/"
#define JS_DIRECTION_PATH "direction"
#define JS_VALUE_PATH "value"

#include "../../app/include/direction.h"

void joystick_init(void);
void joystick_cleanup(void);

Direction joystick_get_input(void);
#endif