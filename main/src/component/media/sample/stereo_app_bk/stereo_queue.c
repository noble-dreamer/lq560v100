/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stereo_queue.h"

ot_s32 stereo_queue_init(stereo_spsc_queue_t *q, ot_u32 capacity)
{
    if (!q || capacity == 0 || (capacity & (capacity - 1)) != 0) {
        return OT_FAILURE; /* capacity must be power of 2 */
    }

    memset(q, 0, sizeof(*q));

    q->slots = (void **)malloc(capacity * sizeof(void *));
    if (!q->slots) {
        return OT_FAILURE;
    }
    memset(q->slots, 0, capacity * sizeof(void *));

    q->capacity  = capacity;
    q->mask      = capacity - 1;
    q->write_idx = 0;
    q->read_idx  = 0;

    return OT_SUCCESS;
}

void stereo_queue_destroy(stereo_spsc_queue_t *q)
{
    if (q && q->slots) {
        free(q->slots);
        q->slots = NULL;
        q->capacity = 0;
    }
}

void stereo_queue_push(stereo_spsc_queue_t *q, void *item)
{
    ot_u32 w;
    while (1) {
        w = q->write_idx;
        /* Full when write catches up to read (count == capacity) */
        if ((w - q->read_idx) < q->capacity) {
            break;
        }
        /* Spin-wait: yield CPU */
        usleep(100);
    }
    q->slots[w & q->mask] = item;
    __sync_synchronize();
    q->write_idx = w + 1;
}

void *stereo_queue_pop(stereo_spsc_queue_t *q)
{
    ot_u32 r;
    void *item;
    while (1) {
        r = q->read_idx;
        if (r != q->write_idx) {
            break;
        }
        /* Spin-wait: yield CPU */
        usleep(100);
    }
    item = q->slots[r & q->mask];
    __sync_synchronize();
    q->read_idx = r + 1;
    return item;
}

ot_bool stereo_queue_try_push(stereo_spsc_queue_t *q, void *item)
{
    ot_u32 w = q->write_idx;
    if ((w - q->read_idx) >= q->capacity) {
        return OT_FALSE;
    }
    q->slots[w & q->mask] = item;
    __sync_synchronize();
    q->write_idx = w + 1;
    return OT_TRUE;
}

void *stereo_queue_try_pop(stereo_spsc_queue_t *q)
{
    ot_u32 r = q->read_idx;
    if (r == q->write_idx) {
        return NULL;
    }
    void *item = q->slots[r & q->mask];
    __sync_synchronize();
    q->read_idx = r + 1;
    return item;
}

ot_bool stereo_queue_is_empty(stereo_spsc_queue_t *q)
{
    return (q->read_idx == q->write_idx) ? OT_TRUE : OT_FALSE;
}

void *stereo_queue_pop_timeout(stereo_spsc_queue_t *q, ot_u32 timeout_ms,
                                volatile ot_bool *quit_flag)
{
    ot_u32 elapsed = 0;
    ot_u32 sleep_us = 1000; /* 1ms per spin iteration */

    while (1) {
        /* Check quit flag first — return NULL when run flag drops to OT_FALSE */
        if (quit_flag && *quit_flag == OT_FALSE) {
            return NULL;
        }

        /* Try non-blocking pop */
        void *item = stereo_queue_try_pop(q);
        if (item) {
            return item;
        }

        /* Timeout check */
        if (timeout_ms > 0 && elapsed >= timeout_ms) {
            return NULL;
        }

        usleep(sleep_us);
        elapsed += sleep_us / 1000;
    }
}
