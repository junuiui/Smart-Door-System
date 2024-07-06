#ifndef _UDP_LISTENER_H
#define _UDP_LISTENER_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <time.h>

#define SIZE 1024
#define PORT 12345

pthread_t udp_get_pthread_id();

void udp_init();

// concatenates premilinary messages that are to appear before desired result
char *displayResponse(char *respond_to_msg, char *prelimMsg);

// Facilitates specific response messages for each valid command
char *verifyCommand(char *myMsg, int sock, struct sockaddr_storage serverAddr);

// Receive messages from socket
void *udp_listen(void *arg);

// Cleanup
void udp_listener_cleanup();

#endif