#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../include/hal/motion_sensor.h"


#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE 1.8

static bool is_initialized = false;

static FILE *f;

void motion_sensor_init(void) {

	printf("motion_sensor - initializing\n");
    assert(!is_initialized);
    is_initialized = true;

}
void motion_sensor_cleanup(void) {
	printf("motion_sensor - cleanup\n");
    assert(is_initialized);
    is_initialized = false;

}

int motion_sensor_get_value()
{
    // Open file
    f = fopen(A2D_FILE_VOLTAGE1, "r");
    if (!f)
    {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }
    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0)
    {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
    // Close file
    fclose(f);
    return a2dReading;
}



double motion_sensor_get_voltage(void)
{
    int a2dReading = motion_sensor_get_value();
    double voltage = (a2dReading/(double)4095) * A2D_VOLTAGE;
    return voltage;
}

double motion_sensor_get_distance(void) {
    
    double voltage = motion_sensor_get_voltage();
    double distance = 29.9 * pow(voltage, -1.7);
    // printf("Distance: %f\n", distance);
    return distance;
}