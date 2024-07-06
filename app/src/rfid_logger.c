
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/rfid_logger.h"
#include "../include/utils.h"
#include "../../hal/include/hal/motion_sensor.h"


/* Variables */
static pthread_t thread_id;
static bool cancel_thread = false;
static rfid_log_queue logQueue;
static _Atomic int num_items = 0;

/* Function prototypes */
static void* rfid_logger_start();


void rfid_logger_init() {

    printf("rfid_logger - initializing\n");

    // Initialize queue
    pthread_mutex_init(&logQueue.lock, NULL);
    logQueue.head = logQueue.tail = NULL;

    // Start main thread
    pthread_create(&thread_id, NULL, rfid_logger_start, NULL);
}


static void* rfid_logger_start() {

    while (!cancel_thread) {

        // Dequeue prints
        //TODO this should be handled by the udp listener
        rfid_log* log = dequeueLog();
        if (log != NULL) {
            printf("%s\n", log->message);
            free(log->message);
            free(log);
        }

        // Sleep to reduce CPU load
        sleep_for_ms(500);
    }
}


int getNumItems() {
    return num_items;
}


/* Get thread for cleanup */
pthread_t rfid_logger_get_pthread_id(){
    return thread_id;
}


void enqueueLog(const char* msg) {

    pthread_mutex_lock(&logQueue.lock);

    rfid_log* newEntry = malloc(sizeof(rfid_log));
    if (newEntry == NULL) {
        printf("rfid_logger enqueueLog() : Failed to allocate memory\n");
        pthread_mutex_unlock(&logQueue.lock);
    }
    newEntry->message = strdup(msg);
    newEntry->next = NULL;

    if (logQueue.tail != NULL) {
        logQueue.tail->next = newEntry;
    }
    logQueue.tail = newEntry;

    if (logQueue.head == NULL) {
        logQueue.head = newEntry;
    }

    num_items++;
    pthread_mutex_unlock(&logQueue.lock);

    printf("enqueueLog() : %s\n", msg);
}


rfid_log* dequeueLog() {
    pthread_mutex_lock(&logQueue.lock);

    if (logQueue.head == NULL) {
        pthread_mutex_unlock(&logQueue.lock);
        return NULL;
    }

    rfid_log* tempLog = logQueue.head;
    logQueue.head = logQueue.head->next;
    if (logQueue.head == NULL) {
        logQueue.tail = NULL;
    }

    num_items--;
    pthread_mutex_unlock(&logQueue.lock);
    return tempLog;
}


void rfid_logger_cleanup() {
    cancel_thread = true;

    rfid_log* log;
    while ((log = dequeueLog()) != NULL) {
        free(log->message);
        free(log);
    }

    pthread_mutex_destroy(&logQueue.lock);

    printf("rfid_logger - cleanup\n");

}