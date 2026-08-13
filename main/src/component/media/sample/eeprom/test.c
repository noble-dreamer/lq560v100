#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "eeprom.h"

static void usage(const char *prog)
{
    fprintf(stderr,
            "Usage:\n"
            "  %s read <bus> <dev_addr_hex> <start_addr_hex> <len>\n"
            "  %s write <bus> <dev_addr_hex> <start_addr_hex> <data_hex...>\n"
            "Examples:\n"
            "  %s read 1 0x50 0x00 16      # read 16 bytes from bus 1, device 0x50, start 0x00\n"
            "  %s write 1 0x50 0x00 0x01 0x02 0x03  # write three bytes then read back\n",
            prog, prog, prog, prog);
}

static int parse_hex_byte(const char *s, uint8_t *out)
{
    char *end;
    long v = strtol(s, &end, 0);
    if (end == s || v < 0 || v > 0xFF) return -1;
    *out = (uint8_t)v;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 6) {
        usage(argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    int bus = atoi(argv[2]);
    uint8_t dev_addr;
    uint8_t start_addr;

    if (parse_hex_byte(argv[3], &dev_addr) < 0) {
        fprintf(stderr, "invalid dev_addr: %s\n", argv[3]);
        return 1;
    }
    if (parse_hex_byte(argv[4], &start_addr) < 0) {
        fprintf(stderr, "invalid start_addr: %s\n", argv[4]);
        return 1;
    }

    if (strcmp(mode, "read") == 0) {
        int len = atoi(argv[5]);
        if (len <= 0) {
            fprintf(stderr, "invalid len: %s\n", argv[5]);
            return 1;
        }
        uint8_t *buf = malloc(len);
        if (!buf) {
            perror("malloc");
            return 1;
        }
        int rc = eeprom_i2c_read((uint8_t)bus, dev_addr, start_addr, buf, (uint8_t)len);
        if (rc < 0) {
            fprintf(stderr, "eeprom_i2c_read failed: %d\n", rc);
            free(buf);
            return 1;
        }
        printf("Read %d bytes:\n", len);
        for (int i = 0; i < len; i++) printf("%02X ", buf[i]);
        printf("\n");
        free(buf);
        return 0;
    } else if (strcmp(mode, "write") == 0) {
        if (argc < 6 + 1) {
            fprintf(stderr, "No data bytes provided for write\n");
            usage(argv[0]);
            return 1;
        }
        int data_count = argc - 5;
        if (data_count > 255) data_count = 255;
        uint8_t *wbuf = malloc(data_count);
        if (!wbuf) { perror("malloc"); return 1; }
        for (int i = 0; i < data_count; i++) {
            if (parse_hex_byte(argv[5 + i], &wbuf[i]) < 0) {
                fprintf(stderr, "invalid data byte: %s\n", argv[5 + i]);
                free(wbuf);
                return 1;
            }
        }

        int rc = eeprom_i2c_write((uint8_t)bus, dev_addr, start_addr, wbuf, (uint8_t)data_count);
        if (rc < 0) {
            fprintf(stderr, "eeprom_i2c_write failed: %d\n", rc);
            free(wbuf);
            return 1;
        }

        /* Read back what we wrote */
        uint8_t *rbuf = malloc(data_count);
        if (!rbuf) { perror("malloc"); free(wbuf); return 1; }
        rc = eeprom_i2c_read((uint8_t)bus, dev_addr, start_addr, rbuf, (uint8_t)data_count);
        if (rc < 0) {
            fprintf(stderr, "eeprom_i2c_read (verify) failed: %d\n", rc);
            free(wbuf); free(rbuf);
            return 1;
        }
        printf("Wrote and read back %d bytes:\n", data_count);
        for (int i = 0; i < data_count; i++) printf("%02X ", rbuf[i]);
        printf("\n");
        free(wbuf);
        free(rbuf);
        return 0;
    }

    usage(argv[0]);
    return 1;
}
