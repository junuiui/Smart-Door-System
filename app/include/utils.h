#ifndef _UTIL_H_
#define _UTIL_H_

void run_command(const char*);
void sleep_for_us(const long long delayInUs);
void sleep_for_ms(const long long delay);
long long get_time_ms(void);

char* file_read(const int, const char*);
void file_write(const char* file, const char* value);

#endif