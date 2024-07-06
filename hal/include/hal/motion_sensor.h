#ifndef _MOTION_SENSOR_
#define _MOTION_SENSOR_

void motion_sensor_init(void);
void motion_sensor_cleanup(void);

int motion_sensor_get_value(void);
double motion_sensor_get_voltage(void);
double motion_sensor_get_distance(void);

#endif 