/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_QUEUE_H__
#define __STEREO_QUEUE_H__

#include "ot_common.h"
#include "stereo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SPSC (Single Producer Single Consumer) lock-free ring buffer.
 * Each element is a pointer-sized payload. The producer spin-waits
 * if full; the consumer spin-waits if empty. Cache-line padded to
 * avoid false sharing on multi-core systems.
 */
#define STEREO_QUEUE_CACHE_LINE  64

typedef struct {
    void              **slots;
    ot_u32              capacity;
    ot_u32              mask;           /* capacity - 1, for fast modulo */
    /* Pad to separate read/write cache lines */
    ot_u8               _pad0[STEREO_QUEUE_CACHE_LINE - 2 * sizeof(ot_u32) - sizeof(void **)];
    volatile ot_u32     write_idx;      /* producer advances */
    ot_u8               _pad1[STEREO_QUEUE_CACHE_LINE - sizeof(ot_u32)];
    volatile ot_u32     read_idx;       /* consumer advances */
    ot_u8               _pad2[STEREO_QUEUE_CACHE_LINE - sizeof(ot_u32)];
} stereo_spsc_queue_t;

/**
 * @brief Initialize an SPSC queue. Capacity must be a power of 2.
 * @param q       Pointer to queue struct
 * @param capacity  Must be power of 2, e.g. 4, 8, 16
 * @return OT_SUCCESS or OT_FAILURE
 */
ot_s32 stereo_queue_init(stereo_spsc_queue_t *q, ot_u32 capacity);

/**
 * @brief Destroy the queue and release slot memory.
 */
void stereo_queue_destroy(stereo_spsc_queue_t *q);

/**
 * @brief Push an element. Spin-waits if full.
 */
void stereo_queue_push(stereo_spsc_queue_t *q, void *item);

/**
 * @brief Pop an element. Spin-waits if empty.
 * @return pointer to the item
 */
void *stereo_queue_pop(stereo_spsc_queue_t *q);

/**
 * @brief Try to push without blocking.
 * @return OT_TRUE on success, OT_FALSE if full.
 */
ot_bool stereo_queue_try_push(stereo_spsc_queue_t *q, void *item);

/**
 * @brief Try to pop without blocking.
 * @return pointer on success, NULL if empty.
 */
void *stereo_queue_try_pop(stereo_spsc_queue_t *q);

/**
 * @brief Check if queue is empty.
 */
ot_bool stereo_queue_is_empty(stereo_spsc_queue_t *q);

/**
 * @brief Pop an element with timeout and quit-flag awareness.
 *        Returns NULL if no item within timeout_ms milliseconds
 *        OR if *quit_flag becomes non-zero.
 * @param q           Queue
 * @param timeout_ms  Max wait in milliseconds (0 = non-blocking try)
 * @param quit_flag   Pointer to volatile flag checked each spin iteration
 * @return pointer on success, NULL on timeout or quit signal.
 */
void *stereo_queue_pop_timeout(stereo_spsc_queue_t *q, ot_u32 timeout_ms,
                                volatile ot_bool *quit_flag);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_QUEUE_H__ */
