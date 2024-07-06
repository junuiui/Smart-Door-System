#include "hal/uptime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to get the uptime in seconds
static double uptime_getSeconds() {
    FILE *fp;
    double uptime_seconds = 0.0;
    char buffer[64];

    // Open /proc/uptime file
    fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        perror("Error opening /proc/uptime");
        exit(EXIT_FAILURE);
    }

    // Read the first number (total uptime) from /proc/uptime
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        uptime_seconds = atof(strtok(buffer, " "));
    }

    // Close the file
    fclose(fp);

    return uptime_seconds;
}

// Function to get the uptime in hours
int uptime_getHour() {
    double uptime_seconds = uptime_getSeconds();
    return (int)(uptime_seconds / 3600);
}

// Function to get the uptime in minutes
int uptime_getMin() {
    double uptime_seconds = uptime_getSeconds();
    return ((int)((uptime_seconds / 60))) % 60;
}

// Function to get the uptime in seconds
int uptime_getSec() {
    double uptime_seconds = uptime_getSeconds();
    return (int)(uptime_seconds) % 60;
}