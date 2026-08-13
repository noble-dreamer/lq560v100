CROSS_COMPILE ?= aarch64-otv02-linux-gnu-
CC = $(CROSS_COMPILE)gcc

CFLAGS = -Wall -O2 -I./tinycrypt
LDFLAGS =

TARGET = ota_prepare
SRCS = ota_prepare.c tinycrypt/ecdsa.c tinycrypt/sha256.c tinycrypt/uECC.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
