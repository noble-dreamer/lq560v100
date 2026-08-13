#ifndef UX_PROTOCOL_H
#define UX_PROTOCOL_H

#include <stdint.h>

// Frame format constants
#define UX_FRAME_HEAD          0x7E
#define UX_ESCAPE_BYTE         0x7D
#define UX_ESCAPE_HEAD         0x02
#define UX_ESCAPE_ESCAPE       0x01

// Maximum message length
#define UX_MAX_MSG_LEN      1024

// Error codes
#define UX_SUCCESS           0
#define UX_ERROR_INIT       -1
#define UX_ERROR_SEND       -2
#define UX_ERROR_RECV       -3
#define UX_ERROR_TIMEOUT    -4
#define UX_ERROR_CHECKSUM   -5
#define UX_ERROR_INVALID    -6

typedef void* ux_handle;

/* init uart port
 * @param: dev_path - uart port device path
 * @param: baudrate - baudrate of uart
 * @return handle of context
 */
ux_handle ux_init(const char *dev_path, int baudrate);

/* pack and send msg to other device
 * @param: h - context handle
 * @param: msg - message contents to be sent
 * @param: len - message length
 * @return error code
 */
int ux_send(ux_handle h, const uint8_t *msg, uint32_t len);

/* recv and parse msg from other device
 * @param: h - context handle
 * @param: msg - message contents received
 * @param: max_len - message buffer length
 * @param: ms - timeout in miliseconds
 * @return error code
 */
int ux_recv(ux_handle h, uint8_t *msg, uint32_t max_len, int ms);

/* release resources and close fd
 * @param: h - context handle
 */
void ux_deinit(ux_handle h);

#endif // UX_PROTOCOL_H
