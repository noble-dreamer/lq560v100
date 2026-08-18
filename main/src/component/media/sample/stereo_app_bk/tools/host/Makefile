# Makefile for host-side stereo security tools (x86_64, OpenSSL)
# Builds: encrypt_model
# Requires: gcc, libssl-dev (OpenSSL development headers)

CC      = gcc

# Try system OpenSSL first, fall back to miniconda3 if not found
OPENSSL_INC := $(shell if [ -f /usr/include/openssl/hmac.h ]; then echo ""; else echo "-I$(HOME)/miniconda3/include"; fi)
OPENSSL_LIB := $(shell if [ -f /usr/lib/x86_64-linux-gnu/libcrypto.so ]; then echo ""; else echo "-L$(HOME)/miniconda3/lib"; fi)

CFLAGS  = -Wall -O2 -I../../ $(OPENSSL_INC)
LDLIBS  = $(OPENSSL_LIB) -lcrypto

# Include the shared stereo_sec_key.h from the stereo_app directory
KEY_HDR_DIR = ../..

TARGETS = encrypt_model

.PHONY: all clean

all: $(TARGETS)

encrypt_model: encrypt_model.c
	$(CC) $(CFLAGS) -I$(KEY_HDR_DIR) -o $@ $< $(LDLIBS)

clean:
	rm -f $(TARGETS)
