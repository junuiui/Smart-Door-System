#ifndef _NEOPIXEL_H
#define _NEOPIXEL_H

#include <stdint.h>

void neopixel_init(void);
void neopixel_cleanup(void);
void neopixel_change_led_color(uint32_t color, int position);
void neopixel_set_all_leds(uint32_t color);
void neopixel_is_door_locked(int flag);

#endif