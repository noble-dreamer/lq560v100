#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

/**
 * @brief Read data from EEPROM using I2C Sequential Read operation
 * 
 * @param i2c_bus I2C bus number (e.g., 0 for /dev/i2c-0)
 * @param dev_addr EEPROM device address (7-bit)
 * @param start_addr Starting register address to read from
 * @param data Pointer to buffer to store read data
 * @param len Number of bytes to read
 * @return 0 on success, negative error code on failure
 */
int eeprom_i2c_read(uint8_t i2c_bus, uint8_t dev_addr, uint8_t start_addr, uint8_t *data, uint8_t len);

/**
 * @brief Write data to EEPROM using I2C Byte Write operation
 * 
 * @param i2c_bus I2C bus number (e.g., 0 for /dev/i2c-0)
 * @param dev_addr EEPROM device address (7-bit)
 * @param start_addr Starting register address to write to
 * @param data Pointer to data to write
 * @param len Number of bytes to write
 * @return 0 on success, negative error code on failure
 */
int eeprom_i2c_write(uint8_t i2c_bus, uint8_t dev_addr, uint8_t start_addr, const uint8_t *data, uint8_t len);

#endif /* EEPROM_H */
