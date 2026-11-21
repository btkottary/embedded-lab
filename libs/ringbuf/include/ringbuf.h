/**
 * @file ringbuf.h
 * @brief Byte ring buffer (FIFO) for passing data between an ISR and main().
 *
 * - The caller provides the storage, so there is no malloc.
 * - Capacity must be a power of two (2, 4, 8, ... 32768): indexes then wrap
 *   with a cheap bit-mask instead of a division.
 * - Safe for ONE producer and ONE consumer without locks (e.g. UART RX ISR
 *   puts, main loop gets), on a single core where 16-bit loads/stores are atomic.
 *
 * Typical use:
 * @code
 *   static uint8_t   rx_storage[64];
 *   static ringbuf_t rx;
 *   ringbuf_init(&rx, rx_storage, sizeof rx_storage);
 *   // ISR:  (void)ringbuf_put(&rx, byte);
 *   // main: uint8_t b; while (ringbuf_get(&rx, &b)) { ... }
 * @endcode
 */
#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t *data;          /**< caller-provided storage                 */
    uint16_t mask;          /**< capacity - 1                            */
    volatile uint16_t head; /**< next write index (only producer writes) */
    volatile uint16_t tail; /**< next read index  (only consumer writes) */
} ringbuf_t;

/**
 * Prepare @p rb to use @p storage.
 * @return false if an argument is NULL or @p capacity is not a power of two
 *         between 2 and 32768.
 */
bool ringbuf_init(ringbuf_t *rb, uint8_t *storage, size_t capacity);

/** Append one byte. @return false (byte dropped) if the buffer is full. */
bool ringbuf_put(ringbuf_t *rb, uint8_t byte);

/** Remove the oldest byte into @p out. @return false if the buffer is empty. */
bool ringbuf_get(ringbuf_t *rb, uint8_t *out);

/** Look at the oldest byte without removing it. @return false if empty. */
bool ringbuf_peek(const ringbuf_t *rb, uint8_t *out);

/** Number of bytes currently stored. */
size_t ringbuf_count(const ringbuf_t *rb);

/** Total number of bytes the buffer can hold. */
size_t ringbuf_capacity(const ringbuf_t *rb);

/** @return true if no bytes are stored. */
bool ringbuf_is_empty(const ringbuf_t *rb);

/** @return true if no more bytes fit. */
bool ringbuf_is_full(const ringbuf_t *rb);

/** Drop all stored bytes (consumer side only). */
void ringbuf_clear(ringbuf_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* RINGBUF_H */
