#ifndef _RFID_HANDLER_H_
#define _RFID_HANDLER_H_

#include <pthread.h>
#include <stdint.h>
#include <time.h>

typedef struct {
    uint32_t uid; // RFID UID
    int allowed; // 1 = allowed, 0 = not allowed (int for future expansion)
    time_t last_seen; // last time a tag was seen
} rfid_tag;

/* Initialize */
void rfid_handler_init();

/* Cleanup */
void rfid_handler_cleanup();

/* Get the number of stored tags */
int rfid_get_num_tags();

/* Get tag info given its uid */
rfid_tag getTag(uint32_t uid);

/* Get all tags */
rfid_tag* rfid_get_all_tags();

/* Get thread ID for cleanup */
pthread_t rfid_handler_get_pthread_id();



#endif // _RFID_HANDLER_H_