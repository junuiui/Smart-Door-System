#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "../../app/include/utils.h"
#include "hal/display.h"

static bool is_initialized = false;
static int i2c_file_desc;

// 0-9 numbers
static struct DisplayValue numbers[] = {
    {0xD0, 0xE1},
    {0xC0, 0x00},
    {0x98, 0x83},
    {0xD8, 0x03},
    {0xC8, 0x22},
    {0x58, 0x23},
    {0x58, 0xA3},
    {0xC0, 0x01},
    {0xD8, 0xE3},
    {0xD8, 0x23},
};


void display_init(void){

	printf("display - initializing\n");
    assert(!is_initialized);
    is_initialized = true;

	file_write(DISPLAY_LEFT "direction", "out");
	file_write(DISPLAY_RIGHT "direction", "out");

	file_write(DISPLAY_LEFT "value", "1");
	file_write(DISPLAY_RIGHT "value", "1");

	run_command("config-pin P9_18 i2c");
    run_command("config-pin P9_17 i2c");

	i2c_file_desc = init_i2c_bus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	write_i2c_reg(i2c_file_desc, REG_DIRA, 0x00);
	write_i2c_reg(i2c_file_desc, REG_DIRB, 0x00);

	write_i2c_reg(i2c_file_desc, REG_OUTA, numbers[0].bank_top);
	write_i2c_reg(i2c_file_desc, REG_OUTB, numbers[0].bank_bottom);
}

void display_cleanup(void){
	printf("display - cleanup\n");
    assert(is_initialized);
    is_initialized = false;

	file_write(DISPLAY_LEFT "value", "0");
	file_write(DISPLAY_RIGHT "value", "0");

	close(i2c_file_desc);
}

void display_value(int number){

    // long long start = get_time_ms();
	// if(number < 10){

	// 	// while(get_time_ms() - start <= ms){

	// 		file_write(DISPLAY_LEFT "value", "0");
	// 		write_i2c_reg(i2c_file_desc, REG_OUTA, numbers[number].bank_top);
	// 		write_i2c_reg(i2c_file_desc, REG_OUTB, numbers[number].bank_bottom);
	// 	// }

	// } else {

		int left = number / 10;
		int right = number % 10;
		// while(get_time_ms() - start <= ms){

			file_write(DISPLAY_LEFT "value", "0");
			file_write(DISPLAY_RIGHT "value", "0");

			write_i2c_reg(i2c_file_desc, REG_OUTA, numbers[left].bank_top  + 0x20);
			write_i2c_reg(i2c_file_desc, REG_OUTB, numbers[left].bank_bottom);

			file_write(DISPLAY_LEFT "value", "1");
			sleep_for_ms(1);

			file_write(DISPLAY_LEFT "value", "0");
			file_write(DISPLAY_RIGHT "value", "0");

			write_i2c_reg(i2c_file_desc, REG_OUTA, numbers[right].bank_top);
			write_i2c_reg(i2c_file_desc, REG_OUTB, numbers[right].bank_bottom);

			file_write(DISPLAY_RIGHT "value", "1");
			sleep_for_ms(1);
		// }
	// }
}

int init_i2c_bus(char* bus, int address) {
	int i2c_file_desc = open(bus, O_RDWR);
	if (i2c_file_desc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2c_file_desc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2c_file_desc;
}

void write_i2c_reg(int i2c_file_desc, unsigned char reg_addr, unsigned char value) {
	unsigned char buff[2];
	buff[0] = reg_addr;
	buff[1] = value;
	int res = write(i2c_file_desc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}

unsigned char read_i2c_reg(int i2c_file_desc, unsigned char reg_addr) {
	// To read a register, must first write the address
	int res = write(i2c_file_desc, &reg_addr, sizeof(reg_addr));
	if (res != sizeof(reg_addr)) {
		perror("Unable to write i2c register.");
		exit(-1);
	}

	// Now read the value and return it
	char value = 0;
	res = read(i2c_file_desc, &value, sizeof(value));
	if (res != sizeof(value)) {
		perror("Unable to read i2c register");
		exit(-1);
	}
	return value;
}