#ifndef _RFID_LOGGER_H_
#define _RFID_LOGGER_H_

#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include "rfid_handler.h"

typedef struct rfid_log {
    char* message;
    struct rfid_log* next;
} rfid_log;

typedef struct rfid_log_queue {
    rfid_log* head;
    rfid_log* tail;
    pthread_mutex_t lock;
} rfid_log_queue;

/* Initialize */
void rfid_logger_init();

/* Cleanup */
void rfid_logger_cleanup();

/* Enqueue */
void enqueueLog(const char* message);

/* Dequeue */
rfid_log* dequeueLog();

/* Get count */
int getNumItems();

/* Get thread ID for cleanup */
pthread_t rfid_logger_get_pthread_id();


#endif // _RFID_LOGGER_H_