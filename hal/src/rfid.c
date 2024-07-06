// RFID module written by Steven
//  See header for references

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>
#include <time.h> // nanosleep

#include "hal/rfid.h"
#include "../../app/include/utils.h"

/* SPI variables */
static bool spi_device_opened = false;
static bool spi_init = false;
static int spiFileDesc;
static int spiMode = 0;
static char* global_spi_device;

/* Pin configuration */
#define RST_PIN "P9_23"
#define GPIO_NUM "49"
#define RST_PIN_CFG "config-pin " RST_PIN " gpio"
#define RST_GPIO_OUT "config-pin " RST_PIN " out"
#define GPIO_CFG "/sys/class/gpio/gpio" GPIO_NUM "/value"

/* Function prototypes */
static void rfid_spi_init(char* device);
static int rfid_spi_transfer(uint8_t *sendBuf, uint8_t *recvBuf, int len);
static void rfid_write(uint8_t addr, uint8_t data);
static uint8_t rfid_read(uint8_t addr);
static void rfid_rdr_init();
static void rfid_fifo_write(uint8_t *writeBuffer, uint8_t writeSize);
static void rfid_fifo_read(uint8_t *readBuffer, uint8_t readSize);
static int rfid_transceive(uint8_t *sendBuf, uint8_t sendLen, uint8_t *recvBuf, uint8_t *recvSize);


/* Initialize the RFID module */
void rfid_init(void) {

    // Configure SPI
    rfid_spi_init(SPI1);
    printf("\tspi_init() return %d\n", spi_init);

    // Configure RST pin
    printf("\tConfiguring RST pin...\n");
    run_command(RST_PIN_CFG);
    run_command(RST_GPIO_OUT);

    // Refresh GPIO pin
    file_write(GPIO_CFG, "0");
    file_write(GPIO_CFG, "1");

    // Set up communication with RC522
    rfid_rdr_init();

    return;
}


/* Initialize the RC522 reader */
static void rfid_rdr_init() {

    // Assuming SPI and GPIO are initialized
    
    // See datasheet §8.5 for prescaler calculations
    // Partial credit (for general process): github.com/miguelbalboa/rfid/blob/master/src/MFRC522.cpp
    rfid_write(TModeReg, (1 << TMODEREG_TAUTO_BIT) | (0x04 << TMODEREG_TPRESCALER_HI_BIT));
    rfid_write(TPrescalerReg, 0x00);
    rfid_write(TReloadRegH, 0x01);
    rfid_write(TReloadRegL, 0x49);

    // Mode settings (§9.3.2.2)
    rfid_write(ModeReg, (1 << MODEREG_TXWAITRF_BIT) | (1 << MODEREG_POLMFIN_BIT)  | (1 << MODEREG_CRCPRESET_BIT));

    // ASK settings (§9.3.2.6)
    rfid_write(TxASKReg, 1 << TXASKREG_FORCE100ASK_BIT);

    // Antenna on
    uint8_t TxControl = rfid_read(TxControlReg);
    rfid_write(TxControlReg, (TxControl | TXCONTROLREG_ANTENNA_ON_MASK));

    printf("Finished rfid_rdr_init()\n");
}


/* Initialize SPI */
static void rfid_spi_init(char* device) {

    // First, open SPI device
    printf("Opening SPI device...\n");
    spiFileDesc = open(device, O_RDWR);
    if (spiFileDesc < 0) {
        printf("ERROR rfid_spi_init() : Cannot open device '%s'\n", device);
    } else {
        spi_device_opened = true;
    }

    // Set the SPI mode
    spiMode = 0;
    int ret = ioctl(spiFileDesc, SPI_IOC_RD_MODE, &spiMode);
    if (ret == -1) {
        printf("ERROR rfid_spi_init() : Can't get SPI mode\n");
    } else {
        spi_init = true;
    }

    // Set SPI device
    global_spi_device = device;

    // printf("\tSet up SPI device '%s' with mode %d\n", device, spiMode);
}


/* This was mostly taken from the SPI guide (which also cites an original source) */
static int rfid_spi_transfer(uint8_t *sendBuf, uint8_t *recvBuf, int len) {

    // Check spiFileDesc
    if (spiFileDesc < 0) {
        printf("rfid_spi_transfer() ERROR: SPI device not opened\n");
        return -1;
    }

    struct spi_ioc_transfer transfer;
    memset(&transfer, 0, sizeof(transfer));

    transfer.tx_buf = (unsigned long)sendBuf;
    transfer.rx_buf = (unsigned long)recvBuf;
    transfer.delay_usecs = 0;
    transfer.len = len;

    int status = ioctl(spiFileDesc, SPI_IOC_MESSAGE(1), &transfer);
    if (status < 0) {
        printf("rfid_spi_transfer() ERROR: SPI transfer failed\n");
    }

    return status;
}


/* This was mostly taken from the SPI guide (which also cites an original source) */
static void rfid_write(uint8_t addr, uint8_t data) {

    // Send buffer
    uint8_t sendBuf[2];
    sendBuf[0] = (addr << 1);
    sendBuf[1] = data;

    // Receive buffer
    uint8_t recvBuf[2] = {0};

    // Execute transfer
    rfid_spi_transfer(sendBuf, recvBuf, 2);
}


/* This was mostly taken from the SPI guide (which also cites an original source) */
static uint8_t rfid_read(uint8_t addr) {

    // Send buffer (signals start read)
    uint8_t sendBuf[2];
    sendBuf[0] = ((1 << 7) | (addr << 1));
    sendBuf[1] = 0x00; // 0x00 signals "stop reading"

    // Receive buffer
    uint8_t recvBuf[2] = {0};

    // Begin transfer
    rfid_spi_transfer(sendBuf, recvBuf, 2);

    // Return the received data
    return recvBuf[1];
}


/* Write to the FIFO buffer (see §8.3) */
static void rfid_fifo_write(uint8_t *buff, uint8_t size) {

    uint8_t payload = size + 1;
    uint8_t sendBuf[payload];
    uint8_t recvBuf[payload];

    // Setup the first byte of the send buffer as the command/address byte
    sendBuf[0] = (REG_WRITE_OP_MASK | (FIFODataReg << REG_ADDR_BIT));

    // Copy buff into sendBuf, starting at sendBuf[1]
    memcpy(sendBuf + 1, buff, size);

    // Perform the SPI transfer
    rfid_spi_transfer(sendBuf, recvBuf, payload);

    // Assuming recvBuf unused
}


/* Read from the FIFO buffer (see §8.3) */
static void rfid_fifo_read(uint8_t *buff, uint8_t size) {

    uint8_t payload = size + 1;
    uint8_t sendBuf[payload];
    uint8_t recvBuf[payload];

    // Send buffer for read operation
    memset(sendBuf, (REG_READ_OP_MASK | (FIFODataReg << REG_ADDR_BIT)), payload - 1);
    sendBuf[payload - 1] = 0; // Set last byte to stop reading

    rfid_spi_transfer(sendBuf, recvBuf, payload);

    // Copy received data to output buffer
    memcpy(buff, recvBuf + 1, size);
}


static int rfid_transceive(uint8_t *sendBuf, uint8_t sendLen, uint8_t *recvBuf, uint8_t *recvLen) {

    // Stop any active command
    rfid_write(CommandReg, Idle);

    // Flush the FIFO buffer
    rfid_write(FIFOLevelReg, 1 << FIFOLEVELREG_FLUSHBUFFER_BIT);

    // Clear the interrupt request register
    rfid_write(ComIrqReg, COMIRQREG_CLEAR_ALL_IRQS);

    // Send to FIFO
    rfid_fifo_write(sendBuf, sendLen);

    // Set ready mode
    rfid_write(CommandReg, Transceive);

    // Begin transceive
    uint8_t bitFraming = rfid_read(BitFramingReg);
    rfid_write(BitFramingReg, bitFraming | (1 << BITFRAMINGREG_STARTSEND_BIT));

    // Variables for polling
    const uint32_t poll_interval_us = 10;
    const uint32_t timeout_us = 25000;
    int status = MI_OK;
    uint32_t elapsed_time = 0;

    // Check timeout/interrupt
    while(true) {

        sleep_for_us(poll_interval_us);
        elapsed_time += poll_interval_us;

        // Get interrupt bits
        uint8_t interruptStatus = rfid_read(ComIrqReg);

        // Check interrupt status
        if(interruptStatus & (COMIRQREG_RXIRQ_MASK | COMIRQREG_IDLEIRQ_MASK)) {
            status = MI_OK;
            break;
        }
        if(interruptStatus & COMIRQREG_TIMERIRQ_MASK) {
            status = MI_ERR;
            break;
        }

        // Check timeout
        if (elapsed_time >= timeout_us) {
            status = MI_TIMEOUT;
            break;
        }
    }

    // Stop transceive
    rfid_write(BitFramingReg, 0x00);
    rfid_write(CommandReg, Idle);
    
    // Get the FIFO data
    uint8_t fifoVal = rfid_read(FIFOLevelReg);
    if(status == MI_OK && recvLen) {
        if(*recvLen > fifoVal) {
            *recvLen = fifoVal;
        }
        if(recvBuf) {
            rfid_fifo_read(recvBuf, *recvLen);
        }
    }

    return status;
}


/* Check for tag, get UID if possible */
int rfid_get_uid(uint64_t *uid) {

    // Check SPI device initialized
    if (!spi_device_opened) {
        printf("rfid_get_uid() ERROR: SPI device not opened\n");
        return MI_ERR;
    }

    // Look for a tag near reader
    rfid_write(BitFramingReg, 7);
    uint8_t sendReq = (uint8_t)PICC_REQA;
    int status =  rfid_transceive(&sendReq, 1, NULL, 0);
    if(status != MI_OK) {
        return status;
    }

    // Begin fetching UID info
    uint8_t sendBuf[2];
    uint8_t recvBuf[5];
    uint8_t recvSize = 5;
    uint8_t recvSizeBefore = 5;

    sendBuf[0] = PICC_SEL_CL1;
    sendBuf[1] = 2 << ANTICOLL_BUF_B2_VALID_BYTES_IN_BUFFER_BIT;

    status = rfid_transceive(sendBuf, 2, recvBuf, &recvSize);

    if (status != MI_OK || recvSize < recvSizeBefore) {
        return MI_ERR;
    }

    // Reconstruct the UID from the received buffer
    *uid = 0;
    for (int i = 0; i < recvSizeBefore; i++) {
        *uid = (*uid << 8) | recvBuf[i];
    }

    return status;
}


/* Cleanup function */
void rfid_cleanup(void) {

    printf("rfid - cleanup\n");
    return;
}
