#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// provided code
void run_command(const char* command) {
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL) break;
    // printf("--> %s", buffer); // Uncomment for debugging
    }

    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

// provided code
void sleep_for_ms(const long long delayInMs){

    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

// adapted from sleep_for_ms (for microseconds)
void sleep_for_us(const long long delayInUs) {

    const long long NS_PER_US = 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInUs * NS_PER_US;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

// provided code
long long get_time_ms(void) {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

// provided code
char* file_read(const int buffer_length, const char *file_name) {
    FILE *pFile = fopen(file_name, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", file_name);
        exit(-1);
    }

    // Read string (line)
    // add extra null terminator
    char* buff = malloc(buffer_length + sizeof(char));
    fgets(buff, buffer_length + sizeof(char), pFile);

    // Close
    fclose(pFile);
    return buff;
}

//provided code
void file_write(const char* file, const char* value){
    FILE* pFile = fopen(file, "w");

    if(pFile == NULL){
        printf("ERROR OPENING %s.", file);
        exit(1);
    }

    int charWritten = fprintf(pFile, "%s", value);
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
        exit(1);
    }

    fclose(pFile);
}