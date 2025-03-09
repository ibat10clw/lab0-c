#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "constant.h"
#include "cpucycles.h"
#include "queue.h"
#include "random.h"

/* Maintain a queue independent from the qtest since
 * we do not want the test to affect the original functionality
 */
static struct list_head *l = NULL;

#define dut_new() ((void) (l = q_new()))

#define dut_size(n)                                \
    do {                                           \
        for (int __iter = 0; __iter < n; ++__iter) \
            q_size(l);                             \
    } while (0)

#define dut_insert_head(s, n)    \
    do {                         \
        int j = n;               \
        while (j--)              \
            q_insert_head(l, s); \
    } while (0)

#define dut_insert_tail(s, n)    \
    do {                         \
        int j = n;               \
        while (j--)              \
            q_insert_tail(l, s); \
    } while (0)

#define dut_free() ((void) (q_free(l)))

static char random_string[N_MEASURES][8];
static int random_string_iter = 0;

/* Implement the necessary queue interface to simulation */
void init_dut(void)
{
    l = NULL;
}

static char *get_random_string(void)
{
    random_string_iter = (random_string_iter + 1) % N_MEASURES;
    return random_string[random_string_iter];
}

void prepare_inputs(uint8_t *input_data, uint8_t *classes)
{
    randombytes(input_data, N_MEASURES * CHUNK_SIZE);
    for (size_t i = 0; i < N_MEASURES; i++) {
        classes[i] = randombit();
        if (classes[i] == 0)
            memset(input_data + (size_t) i * CHUNK_SIZE, 0, CHUNK_SIZE);
    }

    for (size_t i = 0; i < N_MEASURES; ++i) {
        /* Generate random string */
        randombytes((uint8_t *) random_string[i], 7);
        random_string[i][7] = 0;
    }
}
/*
 * Return: False for q_xxx() failed
 */
bool measure(int64_t *before_ticks,
             int64_t *after_ticks,
             uint8_t *input_data,
             int mode)
{
    assert(mode == DUT(insert_head) || mode == DUT(insert_tail) ||
           mode == DUT(remove_head) || mode == DUT(remove_tail));

    for (size_t i = 0; i < N_MEASURES; ++i) {
        char *s, tmp[32] = {0};
        char *sp = NULL;
        int buf_sz = 0;
        uint16_t rn = *(uint16_t *) (input_data + i * CHUNK_SIZE) % 10000;
        // If the random number is zero, use the fixed string "FIXEDSTR" as
        // input. Otherwise, generate a random string and:
        // - Set `sp` to NULL if the random number is odd, or assign `tmp`
        // buffer otherwise.
        // - Set the buffer size `buf_sz` to a value between 0 and 7 based on
        // the random number.
        if (rn == 0) {
            s = "FIXEDSTR";
        } else {
            s = get_random_string();
            sp = tmp;
            buf_sz = (rn % 8) + 1;
        }
        dut_new();
        // Avoid testing remove operations on an empty queue
        // by ensuring 'rn' is non-zero (at least one element present)
        if (mode == DUT_remove_head || mode == DUT_remove_tail)
            rn++;
        dut_insert_head(get_random_string(), rn);
        bool (*insert_func)(struct list_head *, char *) = NULL;
        element_t *(*remove_func)(struct list_head *, char *, size_t) = NULL;

        if (mode == DUT_insert_head || mode == DUT_insert_tail)
            insert_func =
                mode == DUT_insert_head ? q_insert_head : q_insert_tail;
        else if (mode == DUT_remove_head || mode == DUT_remove_tail)
            remove_func =
                mode == DUT_remove_head ? q_remove_head : q_remove_tail;

        int before_size = q_size(l);
        if (insert_func) {
            before_ticks[i] = cpucycles();
            insert_func(l, s);
            after_ticks[i] = cpucycles();
        } else {
            before_ticks[i] = cpucycles();
            element_t *e = remove_func(l, sp, buf_sz);
            after_ticks[i] = cpucycles();
            if (e)
                q_release_element(e);
        }
        int after_size = q_size(l);
        dut_free();

        if (insert_func && before_size != after_size - 1)
            return false;
        else if (remove_func && before_size != after_size + 1)
            return false;
    }
    return true;
}
