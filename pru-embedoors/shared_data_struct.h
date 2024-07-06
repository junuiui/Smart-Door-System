#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_
#include <stdbool.h>
#include <stdint.h>

#define NUM_LED 8

typedef struct {

    _Alignas(4) int value_ms;
    uint32_t colors[NUM_LED];

} shared_data_struct;

#endif