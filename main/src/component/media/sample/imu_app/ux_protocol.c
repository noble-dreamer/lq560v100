#include "ux_protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

#define RX_BULK_SIZE 512

typedef enum {
    RX_STATE_WAIT_HEADER,
    RX_STATE_LENGTH_HIGH,
    RX_STATE_LENGTH_LOW,
    RX_STATE_DATA,
    RX_STATE_CHECKSUM,
    RX_STATE_ESCAPE
} rx_state_t;

typedef struct {
    int fd;
    struct termios old_termios;
    
    // Receive state
    rx_state_t rx_state;
    rx_state_t prev_state;
    uint16_t expected_len;
    uint16_t received_len;
    uint8_t checksum;
    
    // Internal buffer for bulk data
    uint8_t *internal_buffer;
    uint32_t buffer_size;
    uint32_t buffer_head;    // Write position
    uint32_t buffer_tail;    // Read position
    uint32_t buffer_count;   // Bytes in buffer
    
    // Frame extraction buffer
    uint8_t *frame_buffer;
    uint32_t frame_buffer_size;
    uint32_t frame_buffer_pos;
} ux_context_t;

#define DEFAULT_BUFFER_SIZE 8192

#ifdef UX_DEBUG
#define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...) do {} while(0)
#endif

// --- Helper Functions ---

static void update_checksum(uint8_t *checksum, uint8_t byte) {
    *checksum ^= byte;
}

static int send_escaped_byte(int fd, uint8_t byte) {
    if (byte == UX_FRAME_HEAD) {
        uint8_t escape_seq[] = {UX_ESCAPE_BYTE, UX_ESCAPE_HEAD};
        return write(fd, escape_seq, 2) == 2 ? 0 : -1;
    } 
    if (byte == UX_ESCAPE_BYTE) {
        uint8_t escape_seq[] = {UX_ESCAPE_BYTE, UX_ESCAPE_ESCAPE};
        return write(fd, escape_seq, 2) == 2 ? 0 : -1;
    }
    return write(fd, &byte, 1) == 1 ? 0 : -1;
}

static void reset_rx_state(ux_context_t *ctx) {
    ctx->rx_state = RX_STATE_WAIT_HEADER;
    ctx->prev_state = RX_STATE_WAIT_HEADER;
    ctx->expected_len = 0;
    ctx->received_len = 0;
    ctx->checksum = 0;
    ctx->frame_buffer_pos = 0;
}

// --- Circular Buffer Management ---

static int buffer_is_empty(ux_context_t *ctx) {
    return ctx->buffer_count == 0;
}

static int buffer_put_bulk(ux_context_t *ctx, const uint8_t *data, uint32_t len) {
    uint32_t space_available = ctx->buffer_size - ctx->buffer_count;
    if (len > space_available) {
        len = space_available; // Truncate if necessary
    }
    
    for (uint32_t i = 0; i < len; i++) {
        ctx->internal_buffer[ctx->buffer_head] = data[i];
        ctx->buffer_head = (ctx->buffer_head + 1) % ctx->buffer_size;
        ctx->buffer_count++;
    }
    
    return len;
}

static int buffer_get_byte(ux_context_t *ctx, uint8_t *byte) {
    if (buffer_is_empty(ctx)) {
        return 0; // No data available
    }
    
    *byte = ctx->internal_buffer[ctx->buffer_tail];
    ctx->buffer_tail = (ctx->buffer_tail + 1) % ctx->buffer_size;
    ctx->buffer_count--;
    
    return 1;
}

// --- Frame Processing ---

static int handle_escape_state(ux_context_t *ctx, uint8_t byte) {
    uint8_t unescaped_byte;
    
    // Validate escape sequence
    if (byte == UX_ESCAPE_HEAD) {
        unescaped_byte = UX_FRAME_HEAD;
        debug_printf("Unescaped 0x7E\n");
    } else if (byte == UX_ESCAPE_ESCAPE) {
        unescaped_byte = UX_ESCAPE_BYTE;
        debug_printf("Unescaped 0x7D\n");
    } else {
        debug_printf("Invalid escape sequence: 0x%02X\n", byte);
        reset_rx_state(ctx);
        return 0;
    }
    
    // Process unescaped byte based on previous state
    switch (ctx->prev_state) {
        case RX_STATE_LENGTH_HIGH:
            ctx->expected_len = (uint16_t)unescaped_byte << 8;
            update_checksum(&ctx->checksum, unescaped_byte);
            ctx->rx_state = RX_STATE_LENGTH_LOW;
            debug_printf("Length high: 0x%02X (unescaped)\n", unescaped_byte);
            break;
            
        case RX_STATE_LENGTH_LOW:
            ctx->expected_len |= unescaped_byte;
            update_checksum(&ctx->checksum, unescaped_byte);
            debug_printf("Expected length: %d (unescaped)\n", ctx->expected_len);
            
            if (ctx->expected_len > UX_MAX_MSG_LEN) {
                debug_printf("Message too long: %d\n", ctx->expected_len);
                reset_rx_state(ctx);
                return 0;
            }
            
            ctx->rx_state = (ctx->expected_len == 0) ? RX_STATE_CHECKSUM : RX_STATE_DATA;
            break;
            
        case RX_STATE_DATA:
            if (ctx->frame_buffer_pos >= ctx->frame_buffer_size) {
                debug_printf("Frame buffer overflow\n");
                reset_rx_state(ctx);
                return UX_ERROR_RECV;
            }
            
            ctx->frame_buffer[ctx->frame_buffer_pos++] = unescaped_byte;
            update_checksum(&ctx->checksum, unescaped_byte);
            ctx->received_len++;
            
            debug_printf("Data byte[%d]: 0x%02X (unescaped)\n", ctx->received_len - 1, unescaped_byte);
            
            if (ctx->received_len >= ctx->expected_len) {
                ctx->rx_state = RX_STATE_CHECKSUM;
                debug_printf("All data received, waiting for checksum\n");
            } else {
                ctx->rx_state = RX_STATE_DATA;
            }
            break;
            
        case RX_STATE_CHECKSUM:
            debug_printf("Checksum received: 0x%02X, calculated: 0x%02X (unescaped)\n", 
                        unescaped_byte, ctx->checksum);
            
            if (ctx->checksum == unescaped_byte) {
                uint16_t msg_len = ctx->expected_len;
                debug_printf("Frame complete, length: %d\n", msg_len);
                reset_rx_state(ctx);
                return msg_len; // Frame extracted successfully
            } else {
                debug_printf("Checksum error\n");
                reset_rx_state(ctx);
                return UX_ERROR_CHECKSUM;
            }
            
        default:
            debug_printf("Unexpected unescaped byte in state %d\n", ctx->prev_state);
            reset_rx_state(ctx);
            break;
    }
    
    return 0;
}

static int handle_new_frame(ux_context_t *ctx) {
    debug_printf("Frame header found\n");
    ctx->rx_state = RX_STATE_LENGTH_HIGH;
    ctx->checksum = 0;
    ctx->received_len = 0;
    ctx->frame_buffer_pos = 0;
    ctx->expected_len = 0;
    return 0;
}

static int process_byte_from_buffer(ux_context_t *ctx, uint8_t byte) {
    debug_printf("Processing: 0x%02X, state: %d\n", byte, ctx->rx_state);
    
    switch (ctx->rx_state) {
        case RX_STATE_WAIT_HEADER:
            if (byte == UX_FRAME_HEAD) {
                return handle_new_frame(ctx);
            }
            return 0;
            
        case RX_STATE_ESCAPE:
            return handle_escape_state(ctx, byte);
            
        case RX_STATE_LENGTH_HIGH:
            if (byte == UX_FRAME_HEAD) {
                debug_printf("New frame header in LENGTH_HIGH\n");
                reset_rx_state(ctx);
                return handle_new_frame(ctx);
            }
            if (byte == UX_ESCAPE_BYTE) {
                ctx->prev_state = ctx->rx_state;
                ctx->rx_state = RX_STATE_ESCAPE;
                return 0;
            }
            
            ctx->expected_len = (uint16_t)byte << 8;
            update_checksum(&ctx->checksum, byte);
            ctx->rx_state = RX_STATE_LENGTH_LOW;
            debug_printf("Length high: 0x%02X\n", byte);
            return 0;
            
        case RX_STATE_LENGTH_LOW:
            if (byte == UX_FRAME_HEAD) {
                debug_printf("New frame header in LENGTH_LOW\n");
                reset_rx_state(ctx);
                return handle_new_frame(ctx);
            }
            if (byte == UX_ESCAPE_BYTE) {
                ctx->prev_state = ctx->rx_state;
                ctx->rx_state = RX_STATE_ESCAPE;
                return 0;
            }
            
            ctx->expected_len |= byte;
            update_checksum(&ctx->checksum, byte);
            debug_printf("Expected length: %d\n", ctx->expected_len);
            
            if (ctx->expected_len > UX_MAX_MSG_LEN) {
                debug_printf("Message too long: %d\n", ctx->expected_len);
                reset_rx_state(ctx);
                return 0;
            }
            
            ctx->rx_state = (ctx->expected_len == 0) ? RX_STATE_CHECKSUM : RX_STATE_DATA;
            return 0;
            
        case RX_STATE_DATA:
            if (byte == UX_FRAME_HEAD) {
                debug_printf("New frame header in DATA\n");
                reset_rx_state(ctx);
                return handle_new_frame(ctx);
            }
            if (byte == UX_ESCAPE_BYTE) {
                ctx->prev_state = ctx->rx_state;
                ctx->rx_state = RX_STATE_ESCAPE;
                return 0;
            }
            
            if (ctx->frame_buffer_pos >= ctx->frame_buffer_size) {
                debug_printf("Frame buffer overflow\n");
                reset_rx_state(ctx);
                return UX_ERROR_RECV;
            }
            
            ctx->frame_buffer[ctx->frame_buffer_pos++] = byte;
            update_checksum(&ctx->checksum, byte);
            ctx->received_len++;
            
            debug_printf("Data byte[%d]: 0x%02X\n", ctx->received_len - 1, byte);
            
            if (ctx->received_len >= ctx->expected_len) {
                ctx->rx_state = RX_STATE_CHECKSUM;
                debug_printf("All data received, waiting for checksum\n");
            }
            return 0;
            
        case RX_STATE_CHECKSUM:
            if (byte == UX_FRAME_HEAD) {
                debug_printf("New frame header in CHECKSUM\n");
                reset_rx_state(ctx);
                return handle_new_frame(ctx);
            }
            if (byte == UX_ESCAPE_BYTE) {
                ctx->prev_state = ctx->rx_state;
                ctx->rx_state = RX_STATE_ESCAPE;
                return 0;
            }
            
            debug_printf("Checksum received: 0x%02X, calculated: 0x%02X\n", byte, ctx->checksum);
            
            if (ctx->checksum == byte) {
                uint16_t msg_len = ctx->expected_len;
                debug_printf("Frame complete, length: %d\n", msg_len);
                reset_rx_state(ctx);
                return msg_len; // Frame extracted successfully
            } else {
                debug_printf("Checksum error\n");
                reset_rx_state(ctx);
                return UX_ERROR_CHECKSUM;
            }
    }
    
    return 0;
}

// --- Serial Port Setup ---

static int set_baudrate(int fd, int baudrate) {
    struct termios tty;
    speed_t speed;
    
    if (tcgetattr(fd, &tty) != 0) {
        return -1;
    }
    
    switch (baudrate) {
        case 9600:    speed = B9600;    break;
        case 19200:   speed = B19200;   break;
        case 38400:   speed = B38400;   break;
        case 57600:   speed = B57600;   break;
        case 115200:  speed = B115200;  break;
        case 230400:  speed = B230400;  break;
        case 460800:  speed = B460800;  break;
        case 921600:  speed = B921600;  break;
        case 1000000: speed = B1000000; break;
        case 1152000: speed = B1152000; break;
        case 1500000: speed = B1500000; break;
        case 2000000: speed = B2000000; break;
        case 2500000: speed = B2500000; break;
        case 3000000: speed = B3000000; break;
        case 3500000: speed = B3500000; break;
        case 4000000: speed = B4000000; break;
        default:     return -1;
    }
    
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);
    
    tty.c_cflag &= ~(CSIZE | PARENB | PARODD | CSTOPB | CRTSCTS);
    tty.c_cflag |= CS8 | CREAD | CLOCAL;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | 
                     ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    
    return tcsetattr(fd, TCSANOW, &tty);
}

// --- Public API ---

ux_handle ux_init(const char *dev_path, int baudrate) {
    if (!dev_path) {
        return NULL;
    }
    
    ux_context_t *ctx = malloc(sizeof(ux_context_t));
    if (!ctx) {
        return NULL;
    }
    
    memset(ctx, 0, sizeof(ux_context_t));
    
    // Allocate internal buffer
    ctx->buffer_size = DEFAULT_BUFFER_SIZE;
    ctx->internal_buffer = malloc(ctx->buffer_size);
    if (!ctx->internal_buffer) {
        free(ctx);
        return NULL;
    }
    
    reset_rx_state(ctx);
    
    ctx->fd = open(dev_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (ctx->fd < 0) {
        free(ctx->internal_buffer);
        free(ctx);
        return NULL;
    }
    
    if (tcgetattr(ctx->fd, &ctx->old_termios) != 0 ||
        set_baudrate(ctx->fd, baudrate) != 0) {
        close(ctx->fd);
        free(ctx->internal_buffer);
        free(ctx);
        return NULL;
    }
    
    return (ux_handle)ctx;
}

int ux_send(ux_handle h, const uint8_t *msg, uint32_t len) {
    if (!h || !msg || len > UX_MAX_MSG_LEN) {
        return UX_ERROR_INVALID;
    }
    
    ux_context_t *ctx = (ux_context_t *)h;
    uint8_t checksum = 0;
    
    // Send frame header
    uint8_t header = UX_FRAME_HEAD;
    if (write(ctx->fd, &header, 1) != 1) {
        return UX_ERROR_SEND;
    }
    
    // Send length (big-endian) with escaping
    uint8_t len_bytes[] = {(len >> 8) & 0xFF, len & 0xFF};
    
    for (int i = 0; i < 2; i++) {
        update_checksum(&checksum, len_bytes[i]);
        if (send_escaped_byte(ctx->fd, len_bytes[i]) != 0) {
            return UX_ERROR_SEND;
        }
    }
    
    // Send message data with escaping
    for (uint32_t i = 0; i < len; i++) {
        update_checksum(&checksum, msg[i]);
        if (send_escaped_byte(ctx->fd, msg[i]) != 0) {
            return UX_ERROR_SEND;
        }
    }
    
    // Send checksum with escaping
    if (send_escaped_byte(ctx->fd, checksum) != 0) {
        return UX_ERROR_SEND;
    }
    
    return UX_SUCCESS;
}

int ux_recv(ux_handle h, uint8_t *msg, uint32_t max_len, int timeout_ms) {
    if (!h || !msg || max_len == 0) {
        return UX_ERROR_INVALID;
    }
    
    ux_context_t *ctx = (ux_context_t *)h;
    ctx->frame_buffer = msg;
    ctx->frame_buffer_size = max_len;
    
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    
    uint8_t temp_buffer[RX_BULK_SIZE]; // Temporary buffer for bulk reads
    
    while (1) {
        // First, process any data already in the internal buffer
        uint8_t byte;
        while (buffer_get_byte(ctx, &byte)) {
            int result = process_byte_from_buffer(ctx, byte);
            if (result > 0) {
                debug_printf("Frame extracted from buffer, length: %d\n", result);
                return result; // Frame found
            }
            if (result < 0) {
                debug_printf("Frame processing error: %d\n", result);
                // Don't return error immediately, continue processing
            }
        }
        
        // Check timeout
        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        
        long elapsed_ms = (current_time.tv_sec - start_time.tv_sec) * 1000 + 
                         (current_time.tv_usec - start_time.tv_usec) / 1000;
        
        if (elapsed_ms >= timeout_ms) {
            debug_printf("Timeout after %ld ms\n", elapsed_ms);
            return UX_ERROR_TIMEOUT;
        }
        
        // Calculate remaining timeout
        long remaining_ms = timeout_ms - elapsed_ms;
        struct timeval timeout;
        timeout.tv_sec = remaining_ms / 1000;
        timeout.tv_usec = (remaining_ms % 1000) * 1000;
        
        // Wait for new data
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(ctx->fd, &readfds);
        
        int ret = select(ctx->fd + 1, &readfds, NULL, NULL, &timeout);
        if (ret < 0) {
            if (errno == EINTR) continue;
            return UX_ERROR_RECV;
        }
        if (ret == 0) {
            return UX_ERROR_TIMEOUT;
        }
        
        // Read bulk data
        if (FD_ISSET(ctx->fd, &readfds)) {
            ssize_t bytes_read = read(ctx->fd, temp_buffer, sizeof(temp_buffer));
            if (bytes_read < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                return UX_ERROR_RECV;
            }
            if (bytes_read == 0) continue;
            
            debug_printf("Read %zd bytes in bulk\n", bytes_read);
            
            // Store bulk data in internal buffer
            int stored = buffer_put_bulk(ctx, temp_buffer, bytes_read);
            if (stored < bytes_read) {
                debug_printf("Warning: Buffer full, dropped %zd bytes\n", bytes_read - stored);
            }
        }
    }
}

void ux_deinit(ux_handle h) {
    if (!h) {
        return;
    }
    
    ux_context_t *ctx = (ux_context_t *)h;
    tcsetattr(ctx->fd, TCSANOW, &ctx->old_termios);
    close(ctx->fd);
    free(ctx->internal_buffer);
    free(ctx);
}