#include "eeprom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>
#include <time.h>

#define I2C_DEVICE_PATH_MAX 20
/* EEPROM page size (bytes) */
#define EEPROM_PAGE_SIZE    8

/**
 * @brief Read data from EEPROM using I2C Sequential Read operation
 * 
 * Sequential Read:
 * 1. Write register start address: START | DEV_ADDR+W | REG_ADDR
 * 2. Continue read n bytes: START | DEV_ADDR+R | DATA0 | DATA1 | ... | DATAn | STOP
 */
int eeprom_i2c_read(uint8_t i2c_bus, uint8_t dev_addr, uint8_t start_addr, uint8_t *data, uint8_t len)
{
    char i2c_dev_path[I2C_DEVICE_PATH_MAX];
    int fd;
    int ret;

    if (data == NULL || len == 0) {
        fprintf(stderr, "Invalid parameters\n");
        return -EINVAL;
    }

    /* Construct I2C device path */
    snprintf(i2c_dev_path, sizeof(i2c_dev_path), "/dev/i2c-%d", i2c_bus);

    /* Open I2C device */
    fd = open(i2c_dev_path, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return -errno;
    }

    /* Set I2C slave address */
    if (ioctl(fd, I2C_SLAVE, dev_addr) < 0) {
        perror("Failed to set I2C slave address");
        close(fd);
        return -errno;
    }

    /* Use I2C_RDWR ioctl with two messages to perform a repeated-start sequential read:
     * msg[0] - write 1 byte: register address
     * msg[1] - read 'len' bytes into data
     */
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data ioctl_data;

    msgs[0].addr  = dev_addr;
    msgs[0].flags = 0; /* write */
    msgs[0].len   = 1;
    msgs[0].buf   = &start_addr;

    msgs[1].addr  = dev_addr;
    msgs[1].flags = I2C_M_RD; /* read */
    msgs[1].len   = len;
    msgs[1].buf   = data;

    ioctl_data.msgs = msgs;
    ioctl_data.nmsgs = 2;

    ret = ioctl(fd, I2C_RDWR, &ioctl_data);
    if (ret < 0) {
        perror("I2C_RDWR ioctl sequential read failed");
        close(fd);
        return -errno;
    }

    close(fd);
    return 0;
}

/**
 * @brief Write data to EEPROM using I2C Byte Write operation
 * 
 * Byte Write:
 * START | DEV_ADDR+W | REG_ADDR | DATA0 | DATA1 | ... | DATAn | STOP
 * 
 * Note: This function writes bytes one at a time with a delay between writes
 * to accommodate EEPROM write cycle time (typically 5-10ms)
 */
int eeprom_i2c_write(uint8_t i2c_bus, uint8_t dev_addr, uint8_t start_addr, const uint8_t *data, uint8_t len)
{
    char i2c_dev_path[I2C_DEVICE_PATH_MAX];
    int fd;
    int ret;

    if (data == NULL || len == 0) {
        fprintf(stderr, "Invalid parameters\n");
        return -EINVAL;
    }

    /* Construct I2C device path */
    snprintf(i2c_dev_path, sizeof(i2c_dev_path), "/dev/i2c-%d", i2c_bus);

    /* Open I2C device */
    fd = open(i2c_dev_path, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return -errno;
    }

    /* Set I2C slave address */
    if (ioctl(fd, I2C_SLAVE, dev_addr) < 0) {
        perror("Failed to set I2C slave address");
        close(fd);
        return -errno;
    }

    /* Page write: write up to EEPROM_PAGE_SIZE bytes in a single transaction, without
     * crossing page boundaries. After each page write, poll for ACK (write completion).
     */
    {
        uint32_t offset = 0;
        while (offset < len) {
            uint8_t current_addr = start_addr + offset;
            /* Calculate remaining bytes in current page */
            uint8_t page_offset = current_addr % EEPROM_PAGE_SIZE;
            uint8_t space_in_page = EEPROM_PAGE_SIZE - page_offset;
            uint8_t remaining = len - offset;
            uint8_t chunk = remaining < space_in_page ? remaining : space_in_page;

            /* Prepare write buffer: [reg_addr, data0, data1, ...] */
            uint8_t page_buf[1 + EEPROM_PAGE_SIZE];
            page_buf[0] = current_addr;
            memcpy(&page_buf[1], &data[offset], chunk);

            struct i2c_msg wmsg;
            struct i2c_rdwr_ioctl_data wdata;

            wmsg.addr  = dev_addr;
            wmsg.flags = 0; /* write */
            wmsg.len   = 1 + chunk;
            wmsg.buf   = page_buf;

            wdata.msgs  = &wmsg;
            wdata.nmsgs = 1;

            ret = ioctl(fd, I2C_RDWR, &wdata);
            if (ret < 0) {
                perror("I2C_RDWR ioctl page write failed");
                close(fd);
                return -errno;
            }
            /* Wait for EEPROM write cycle to complete */
            usleep(5000); /* 5 ms delay for write cycle */
            offset += chunk;
        }
    }

    close(fd);
    return 0;
}
