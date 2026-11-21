/**
 * @file ringbuf.c
 * @brief Single-producer / single-consumer byte ring buffer.
 *
 * Design: head and tail are free-running 16-bit counters. The number of
 * stored bytes is (head - tail) in modulo-2^16 arithmetic, and the storage
 * index is (counter & mask). Because capacity <= 32768, the full buffer
 * (count == capacity) can never be confused with the empty one (count == 0),
 * so no storage slot is wasted.
 */
#include "ringbuf.h"

#define RINGBUF_MAX_CAPACITY 32768U

static bool is_power_of_two(size_t n)
{
    return (n != 0U) && ((n & (n - 1U)) == 0U);
}

bool ringbuf_init(ringbuf_t *rb, uint8_t *storage, size_t capacity)
{
    if ((rb == NULL) || (storage == NULL) || (capacity < 2U) || (capacity > RINGBUF_MAX_CAPACITY) ||
        !is_power_of_two(capacity))
    {
        return false;
    }
    rb->data = storage;
    rb->mask = (uint16_t)(capacity - 1U);
    rb->head = 0U;
    rb->tail = 0U;
    return true;
}

size_t ringbuf_capacity(const ringbuf_t *rb)
{
    return (size_t)rb->mask + 1U;
}

size_t ringbuf_count(const ringbuf_t *rb)
{
    return (uint16_t)(rb->head - rb->tail);
}

bool ringbuf_is_empty(const ringbuf_t *rb)
{
    return ringbuf_count(rb) == 0U;
}

bool ringbuf_is_full(const ringbuf_t *rb)
{
    return ringbuf_count(rb) == ringbuf_capacity(rb);
}

bool ringbuf_put(ringbuf_t *rb, uint8_t byte)
{
    if (ringbuf_is_full(rb))
    {
        return false;
    }
    rb->data[rb->head & rb->mask] = byte;
    rb->head = (uint16_t)(rb->head + 1U); /* publish AFTER the data is written */
    return true;
}

bool ringbuf_peek(const ringbuf_t *rb, uint8_t *out)
{
    if ((out == NULL) || ringbuf_is_empty(rb))
    {
        return false;
    }
    *out = rb->data[rb->tail & rb->mask];
    return true;
}

bool ringbuf_get(ringbuf_t *rb, uint8_t *out)
{
    if (!ringbuf_peek(rb, out))
    {
        return false;
    }
    rb->tail = (uint16_t)(rb->tail + 1U); /* free the slot AFTER reading it */
    return true;
}

void ringbuf_clear(ringbuf_t *rb)
{
    rb->tail = rb->head;
}
