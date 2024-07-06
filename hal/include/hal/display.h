#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x20

#define DISPLAY_LEFT "/sys/class/gpio/gpio61/"
#define DISPLAY_RIGHT "/sys/class/gpio/gpio44/"

#define REG_DIRA 0x02
#define REG_DIRB 0x03
#define REG_OUTA 0x00
#define REG_OUTB 0x01

struct DisplayValue {

    int bank_top;
    int bank_bottom;

};

void display_init(void);
void display_cleanup(void);
void display_value(int value);

int init_i2c_bus(char* bus, int address);
void write_i2c_reg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
unsigned char read_i2c_reg(int i2cFileDesc, unsigned char regAddr);

#endif