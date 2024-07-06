#include "udp_listener.h"
#include "hal/uptime.h"
#include "rfid_handler.h"
#include "rfid_logger.h"
#include "time.h"

#include <fcntl.h> // udp blocking
#include <errno.h> // udp blocking

static char *sliceString(const char *str, int start, int end);

static bool stopping = false;
static pthread_t thread_id;
static int prevNum = 0;
static int count = 0;
static char *displayError(char *respond_to_msg)
{
	memset(respond_to_msg, 0, sizeof(char) * SIZE);
	sprintf(respond_to_msg, "ERROR: Invalid command\n");
	return respond_to_msg;
}

pthread_t udp_get_pthread_id()
{
	return thread_id;
}

void udp_init()
{
	pthread_create(&thread_id, NULL, udp_listen, NULL);
}

static char *getUptime()
{
	// Allocate memory for the response
	char *uptimeString = (char *)malloc(SIZE * sizeof(char));
	if (uptimeString == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}

	// Get the uptime components
	int hours = uptime_getHour();
	int minutes = uptime_getMin();
	int seconds = uptime_getSec();

	// Format the uptime string
	sprintf(uptimeString, "%2d:%2d:%2d\n", hours, minutes, seconds);

	return uptimeString;
}

static char *get_logs()
{
	// Allocate memory for the response
	char *sample = (char *)malloc(SIZE * 2 * sizeof(char));
	if (sample == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}

	// Initialize sample to an empty string
	sample[0] = '\0';

	char *copied = (char *)malloc(SIZE * 2 * sizeof(char));
	if (copied == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}
	int curNum = getNumItems();
	rfid_log *log;

	if (curNum > 0)
	{
		// Initialize copied to an empty string
		copied[0] = '\0';
		log = dequeueLog();
		strcat(copied, log->message);
		printf("COPIED: %s\n", copied);
	}
	else
	{
		// dummy
		strcat(sample, "[RFID]: < 0:00:00 > UID 0x00000000 ");
		return sample;
	}

	int startNum = 36; // index of U
	if (copied[21] == 'K')
	{
		startNum += 2;
	}
	else if (copied[21] == 'N')
	{
		// do nothing
	}

	char *timeStr = sliceString(copied, 11, 18);
	char *uid = sliceString(copied, startNum, (startNum + 7));
	char allowed = copied[strlen(copied) - 3];

	printf("TIME  : %s\n", timeStr);
	printf("UID   : %s\n", uid);
	printf("STATUS: %c\n", allowed);

	// Format: [RFID] < 00 : 00: 00 > : UDI 0x12345678 ACCEPTED
	strcat(sample, "[RFID]: <");
	strcat(sample, timeStr);
	free(timeStr);
	strcat(sample, "> UID 0x");
	strcat(sample, uid);
	free(uid);

	if (allowed == '1')
	{

		strcat(sample, " GRANTED\n");
	}
	else
	{
		strcat(sample, " DENIED\n");
	}

	return sample;
}

char *verifyCommand(char *myMsg, int sock, struct sockaddr_storage serverAddr)
{

	size_t commandLength = strlen(myMsg);
	if (commandLength > 0 && myMsg[commandLength - 1] == '\n')
	{
		myMsg[commandLength - 1] = '\0';
	}

	if (strcmp(myMsg, "getHistory") == 0)
	{
		return get_logs();
	}
	else if (strcmp(myMsg, "getUptime") == 0)
	{
		return getUptime();
	}

	return NULL;
}

void *udp_listen(void *arg)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0); // socket initailization
	struct sockaddr_storage serverAddr;
	char myMsg[1024];
	int b;

	// binding
	struct sockaddr_in sockName;
	memset((char *)&sockName, 0, sizeof(sockName));
	sockName.sin_family = AF_INET;
	sockName.sin_port = htons(PORT);
	sockName.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t serverAddrSize;

	if (bind(sock, (struct sockaddr *)&sockName, sizeof(sockName)))
	{
		perror("binding failed");
		exit(1);
	}

	serverAddrSize = sizeof(serverAddr);

	// Set socket to non-blocking
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl get flags");
		exit(EXIT_FAILURE);
	}
	flags |= O_NONBLOCK;
	if (fcntl(sock, F_SETFL, flags) == -1)
	{
		perror("fcntl set non-blocking");
		exit(EXIT_FAILURE);
	}

	// send msg to server
	printf("starting udp loop\n");
	while (!stopping)
	{

		// Get info
		// printf("udp loop\n");
		b = recvfrom(sock, myMsg, 1024, 0, (struct sockaddr *)&serverAddr, &serverAddrSize);
		// printf("udp loop 2\n");

		if (b > 0)
		{
			myMsg[b] = '\0';
			char *msgsss = verifyCommand(myMsg, sock, serverAddr);
			if (strcmp(msgsss, "Terminated") == 0)
			{
				sendto(sock, msgsss, 1024, 0,
					   (struct sockaddr *)&serverAddr, serverAddrSize);
				free(msgsss);
				break;
			}
			sendto(sock, msgsss, 1024, 0, (struct sockaddr *)&serverAddr,
				   serverAddrSize);
			free(msgsss);
		}
	}

	close(sock);
	exit(EXIT_SUCCESS);
}

static char *sliceString(const char *str, int start, int end)
{
	// Calculate the length of the sliced substring
	int length = end - start + 1;

	// Allocate memory for the sliced substring (+1 for the null terminator)
	char *sliced = (char *)malloc(length + 1);
	if (sliced == NULL)
	{
		printf("Memory allocation failed.\n");
		exit(1);
	}

	// Copy the sliced substring into the allocated memory
	strncpy(sliced, str + start, length);

	// Add the null terminator
	sliced[length] = '\0';

	return sliced;
}

void udp_listener_cleanup()
{
	stopping = true;
}
