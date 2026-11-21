/**
 * @file test_ringbuf.c
 * @brief Unit tests for libs/ringbuf, run on the PC with Unity.
 */
#include "ringbuf.h"
#include "unity.h"

#define CAPACITY 8U

static uint8_t s_storage[CAPACITY];
static ringbuf_t s_rb;

void setUp(void);
void tearDown(void);

void setUp(void)
{
    TEST_ASSERT_TRUE(ringbuf_init(&s_rb, s_storage, CAPACITY));
}

void tearDown(void)
{
}

static void test_init_rejects_bad_arguments(void)
{
    ringbuf_t rb;
    uint8_t buf[16];
    TEST_ASSERT_FALSE(ringbuf_init(NULL, buf, sizeof buf));
    TEST_ASSERT_FALSE(ringbuf_init(&rb, NULL, sizeof buf));
    TEST_ASSERT_FALSE(ringbuf_init(&rb, buf, 0U));
    TEST_ASSERT_FALSE(ringbuf_init(&rb, buf, 1U));
    TEST_ASSERT_FALSE(ringbuf_init(&rb, buf, 12U)); /* not a power of two */
    TEST_ASSERT_TRUE(ringbuf_init(&rb, buf, 16U));
}

static void test_new_buffer_is_empty(void)
{
    uint8_t b;
    TEST_ASSERT_TRUE(ringbuf_is_empty(&s_rb));
    TEST_ASSERT_FALSE(ringbuf_is_full(&s_rb));
    TEST_ASSERT_EQUAL_UINT(0U, ringbuf_count(&s_rb));
    TEST_ASSERT_EQUAL_UINT(CAPACITY, ringbuf_capacity(&s_rb));
    TEST_ASSERT_FALSE(ringbuf_get(&s_rb, &b));
    TEST_ASSERT_FALSE(ringbuf_peek(&s_rb, &b));
}

static void test_bytes_come_out_in_order(void)
{
    uint8_t b;
    TEST_ASSERT_TRUE(ringbuf_put(&s_rb, 0x11));
    TEST_ASSERT_TRUE(ringbuf_put(&s_rb, 0x22));
    TEST_ASSERT_TRUE(ringbuf_put(&s_rb, 0x33));
    TEST_ASSERT_EQUAL_UINT(3U, ringbuf_count(&s_rb));

    TEST_ASSERT_TRUE(ringbuf_get(&s_rb, &b));
    TEST_ASSERT_EQUAL_HEX8(0x11, b);
    TEST_ASSERT_TRUE(ringbuf_get(&s_rb, &b));
    TEST_ASSERT_EQUAL_HEX8(0x22, b);
    TEST_ASSERT_TRUE(ringbuf_get(&s_rb, &b));
    TEST_ASSERT_EQUAL_HEX8(0x33, b);
    TEST_ASSERT_TRUE(ringbuf_is_empty(&s_rb));
}

static void test_full_buffer_rejects_put_and_uses_every_slot(void)
{
    for (unsigned i = 0U; i < CAPACITY; i++)
    {
        TEST_ASSERT_TRUE(ringbuf_put(&s_rb, (uint8_t)i));
    }
    TEST_ASSERT_TRUE(ringbuf_is_full(&s_rb));
    TEST_ASSERT_EQUAL_UINT(CAPACITY, ringbuf_count(&s_rb));
    TEST_ASSERT_FALSE(ringbuf_put(&s_rb, 0xFF));
}

static void test_peek_does_not_remove(void)
{
    uint8_t b = 0U;
    TEST_ASSERT_TRUE(ringbuf_put(&s_rb, 0xAB));
    TEST_ASSERT_TRUE(ringbuf_peek(&s_rb, &b));
    TEST_ASSERT_EQUAL_HEX8(0xAB, b);
    TEST_ASSERT_EQUAL_UINT(1U, ringbuf_count(&s_rb));
}

static void test_wraps_around_many_times(void)
{
    /* Push 70 000 bytes through an 8-byte buffer: exercises index wrap and the
     * 16-bit counter overflow at 65 536. */
    uint8_t b;
    for (unsigned i = 0U; i < 70000U; i++)
    {
        TEST_ASSERT_TRUE(ringbuf_put(&s_rb, (uint8_t)i));
        TEST_ASSERT_TRUE(ringbuf_get(&s_rb, &b));
        TEST_ASSERT_EQUAL_HEX8((uint8_t)i, b);
    }
    TEST_ASSERT_TRUE(ringbuf_is_empty(&s_rb));
}

static void test_clear_empties_buffer(void)
{
    TEST_ASSERT_TRUE(ringbuf_put(&s_rb, 1));
    TEST_ASSERT_TRUE(ringbuf_put(&s_rb, 2));
    ringbuf_clear(&s_rb);
    TEST_ASSERT_TRUE(ringbuf_is_empty(&s_rb));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_rejects_bad_arguments);
    RUN_TEST(test_new_buffer_is_empty);
    RUN_TEST(test_bytes_come_out_in_order);
    RUN_TEST(test_full_buffer_rejects_put_and_uses_every_slot);
    RUN_TEST(test_peek_does_not_remove);
    RUN_TEST(test_wraps_around_many_times);
    RUN_TEST(test_clear_empties_buffer);
    return UNITY_END();
}
