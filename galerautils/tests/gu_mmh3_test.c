// Copyright (C) 2012 Codership Oy <info@codership.com>

// $Id$

#include "gu_mmh3_test.h"

#include "../src/gu_mmh3.h"
#include "../src/gu_log.h"
#include "../src/gu_print_buf.h"

/* This is to verify all tails plus block + all tails. Max block is 16 bytes */
static const char const test_input[] = "0123456789ABCDEF0123456789abcde";

typedef struct hash32 { uint8_t h[4]; } hash32_t;

#define NUM_32_TESTS 8 /* 0 to 7 bytes */

static const hash32_t const
test_output32[NUM_32_TESTS] =
{
    {{ 0x0b, 0x7c, 0x3e, 0xab }}, /* ''        */
    {{ 0xba, 0xeb, 0x75, 0x97 }}, /* '0'       */
    {{ 0x5d, 0x5c, 0x21, 0x60 }}, /* '01'      */
    {{ 0x4b, 0xff, 0x61, 0x41 }}, /* '012'     */
    {{ 0x35, 0x3b, 0x57, 0xca }}, /* '0123'    */
    {{ 0x09, 0xdd, 0x77, 0xf9 }}, /* '01234'   */
    {{ 0x1f, 0x3c, 0x29, 0x7b }}, /* '012345'  */
    {{ 0xe1, 0xbe, 0x2d, 0xce }}  /* '0123456' */
};

typedef struct hash128 { uint8_t h[16]; } hash128_t;

#define NUM_128_TESTS 32 /* 0 to 31 bytes */

static const hash128_t const
test_output128[NUM_128_TESTS] =
{
    {{ 0xa9,0xce,0x5a,0x56,0x0c,0x0b,0xf7,0xd6,0x63,0x4f,0x6f,0x81,0x0e,0x0b,0xf2,0x0a }},
    {{ 0x72,0xa1,0x46,0xa3,0x73,0x03,0x49,0x85,0x30,0xb9,0x52,0xaa,0x3b,0x00,0xad,0x23 }},
    {{ 0x4f,0x32,0xa2,0x15,0x91,0x00,0xea,0xaa,0x59,0x90,0x48,0x30,0xe5,0x86,0x50,0xee }},
    {{ 0x55,0xfe,0x86,0x3b,0x9c,0x67,0xc6,0xee,0x5c,0x06,0x34,0xd0,0xe5,0x15,0xfb,0xdd }},
    {{ 0x3a,0x50,0x35,0xe5,0x72,0x75,0xa5,0x5e,0x46,0x3d,0x0e,0x23,0xbb,0x17,0x5a,0x66 }},
    {{ 0x3b,0xff,0xb5,0x1a,0x93,0x0c,0x77,0x9a,0x40,0x5f,0x62,0x0c,0x40,0x15,0x0b,0x6e }},
    {{ 0x7c,0xf8,0xf9,0xd2,0xfa,0x5a,0x8b,0x51,0x65,0x3c,0xa5,0x0e,0xa2,0xca,0x0a,0x87 }},
    {{ 0x95,0x69,0x33,0x98,0xe4,0xb2,0x2a,0x21,0xd4,0x23,0x21,0x80,0xb1,0x00,0x46,0xbb }},
    {{ 0x92,0xca,0xd3,0xbb,0x39,0x16,0x96,0xb5,0x3a,0x61,0x58,0x53,0xbb,0xf8,0xc4,0xb0 }},
    {{ 0x36,0xf0,0xa3,0xc8,0xdc,0x5e,0x46,0x20,0x12,0xcf,0xad,0x3f,0xda,0xd5,0x95,0x7a }},
    {{ 0xb9,0x71,0x76,0x54,0xd3,0x74,0x9b,0x31,0x93,0xb2,0xd9,0xbf,0xad,0x78,0x49,0x7e }},
    {{ 0x39,0x75,0xc6,0x34,0x38,0x65,0x60,0x32,0xb1,0xa3,0x02,0xd2,0xba,0x47,0x0b,0xc3 }},
    {{ 0x37,0xcd,0xe3,0x34,0x7d,0x2d,0xa4,0xdc,0xf3,0x51,0xd1,0x1e,0x46,0xb8,0x1a,0xd4 }},
    {{ 0xa0,0xf6,0xff,0xc6,0xcd,0x50,0xdf,0xa2,0x59,0x36,0x8d,0xdf,0x09,0x57,0x14,0x7b }},
    {{ 0xeb,0x58,0x42,0xca,0x56,0xb5,0x94,0x16,0x10,0x86,0x38,0x5b,0x2c,0x4a,0x13,0x84 }},
    {{ 0x5d,0xee,0x3a,0x5b,0x45,0x5f,0x92,0x7d,0x42,0x91,0x8a,0x7b,0xb6,0xc7,0xde,0xd9 }},
    {{ 0x63,0xff,0xe5,0x55,0x38,0x3d,0xd6,0x5d,0xa4,0xad,0xcb,0xf6,0x0a,0xc3,0xd9,0x12 }},
    {{ 0x86,0x15,0xd3,0x5a,0x47,0x81,0x3f,0xea,0x6b,0xbc,0x3b,0x82,0xd0,0x49,0xda,0x5d }},
    {{ 0xb7,0x41,0xc9,0xf5,0x94,0x3f,0x91,0xa5,0x56,0x68,0x9c,0x12,0xc7,0xa1,0xd9,0x45 }},
    {{ 0xb7,0x7c,0x2f,0x60,0xe3,0x2b,0x6a,0xd6,0x5e,0x24,0x6c,0xaf,0x8c,0x83,0x99,0xc7 }},
    {{ 0x62,0xdb,0xad,0xab,0xda,0x51,0x82,0x0b,0x04,0xe6,0x7a,0x88,0xaa,0xae,0xfd,0xce }},
    {{ 0x70,0x89,0xd2,0x6a,0x35,0x80,0x19,0xa4,0x71,0x0e,0x5c,0x68,0x33,0xf5,0x0c,0x67 }},
    {{ 0x05,0xb3,0x50,0x50,0xbe,0x8d,0xaa,0x6e,0x32,0x02,0x1b,0x5e,0xe6,0xb7,0x5f,0x72 }},
    {{ 0x85,0x60,0x7c,0x7a,0xdf,0xaa,0x67,0xc6,0xed,0x3e,0x7e,0x13,0x84,0x2c,0xd4,0x28 }},
    {{ 0x51,0x4a,0xe3,0x56,0xe0,0x5f,0x7d,0x42,0xfb,0x41,0xec,0xfe,0xff,0xa4,0x74,0x13 }},
    {{ 0xb8,0xc0,0xc1,0x01,0xc2,0x74,0xbb,0x84,0xc8,0xca,0x16,0x9c,0x6b,0xf3,0x3e,0x4d }},
    {{ 0xab,0xd0,0x4a,0xc5,0xa4,0xc8,0xce,0xf4,0xf2,0xf5,0x2f,0xdc,0x22,0x4f,0x20,0xda }},
    {{ 0x36,0x25,0x28,0x74,0xf0,0x4c,0x36,0x38,0xd2,0x9a,0x64,0xf8,0x11,0xcf,0xaf,0x28 }},
    {{ 0x8b,0x79,0x18,0x09,0x14,0x19,0x3c,0xa0,0x5b,0x62,0x4d,0x09,0x18,0xdd,0x6a,0x89 }},
    {{ 0xc0,0xae,0x4f,0x67,0x45,0x01,0x00,0xb7,0x75,0xc5,0x1c,0x56,0xdf,0x55,0x7c,0x04 }},
    {{ 0xcd,0x5a,0xda,0xea,0xbc,0xfb,0x8d,0xc7,0x8a,0xd3,0xc6,0x70,0x12,0x34,0x82,0x84 }},
    {{ 0x69,0x53,0x0d,0xc3,0x4d,0xd4,0x33,0xe9,0x00,0x1b,0x27,0x06,0x27,0x7f,0x48,0xf7 }}
};

typedef void (*hash_f_t) (const void* key, int len, uint32_t seed, void* out);

/* Verification code from the original SMHasher test suite */
static void
smhasher_verification (hash_f_t hash, size_t hashbytes, hash32_t* res)
{
    ssize_t const n_tests = 256;
    uint8_t key[n_tests];
    uint8_t hashes[hashbytes * n_tests];
    uint8_t final[hashbytes];

    /* Hash keys of the form {0}, {0,1}, {0,1,2}... up to N=255,using 256-N as
     * the seed */
    ssize_t i;
    for(i = 0; i < n_tests; i++)
    {
        key[i] = (uint8_t)i;
        hash (key, i, n_tests - i, &hashes[i * hashbytes]);
    }

    /* Then hash the result array */
    hash (hashes, hashbytes * n_tests, 0, final);

    memcpy (res, final, sizeof(*res));
}

static hash32_t
smhasher_checks[3] =
{
    {{ 0xE3, 0x7E, 0xF5, 0xB0 }}, /* mmh3_32      */
    {{ 0x2A, 0xE6, 0xEC, 0xB3 }}, /* mmh3_x86_128 */
    {{ 0x69, 0xBA, 0x84, 0x63 }}  /* mmh3_x64_128 */
};

#if 0
/* Verification code for Galera variant of MurmurHash3 - with constant seed */
static void
gu_verification (hash_f_t hash, size_t hashbytes, hash128_t* res)
{
    ssize_t const n_tests = 256;
    uint8_t key[n_tests];
    uint8_t hashes[hashbytes * n_tests];
    uint8_t final[hashbytes];

    /* Hash keys of the form {0}, {0,1}, {0,1,2}... up to N=255, using constant
     * seed */
    ssize_t i;
    for(i = 0; i < n_tests; i++)
    {
        key[i] = (uint8_t)i;
        hash (key, i, n_tests - i, &hashes[i * hashbytes]);
    }

    /* Then hash the result array */
    hash (hashes, hashbytes * n_tests, 0, final);

    memcpy (res, final, hashbytes);
}

static hash128_t
gu_checks[3] =
{
    {{ 0xE3, 0x7E, 0xF5, 0xB0, }}, /* mmh3_32      */
    {{ 0x2A, 0xE6, 0xEC, 0xB3, }}, /* mmh3_x86_128 */
    {{ 0x69, 0xBA, 0x84, 0x63, }}  /* mmh3_x64_128 */
};
#endif

/* returns true if check fails */
static bool
check (const void* const exp, const void* const got, ssize_t size)
{
    if (memcmp (exp, got, size))
    {
        ssize_t str_size = size * 2.2 + 1;
        char c[str_size], r[str_size];

        gu_print_buf (exp, size, c, sizeof(c), false);
        gu_print_buf (got, size, r, sizeof(r), false);

        gu_info ("expected MurmurHash3:\n%s\nfound:\n%s\n", c, r);

        return true;
    }

    return false;
}

START_TEST (gu_mmh32_test)
{
    int i;
    hash32_t out;

    smhasher_verification (gu_mmh3_32, sizeof(hash32_t), &out);
    fail_if (check (&smhasher_checks[0], &out, sizeof(out)),
             "gu_mmh3_32 failed.");

    for (i = 0; i < NUM_32_TESTS; i++)
    {
        uint32_t out = gu_mmh32 (test_input, i);
        fail_if(check (&test_output32[i], &out, sizeof(out)),
                "gu_mmh32() failed at step %d",i);
    }
}
END_TEST

#if 0 /* x86 variant is faulty and unsuitable for short keys, ignore */
START_TEST (gu_mmh128_x86_test)
{
    int i;
    hash32_t out32;

    smhasher_verification (gu_mmh3_x86_128, sizeof(hash128_t), &out32);
    fail_if (check (&smhasher_checks[1], &out32, sizeof(out32)),
             "gu_mmh3_x86_128 failed.");

    for (i = 0; i < NUM_128_TESTS; i++)
    {
        hash128_t out;
        gu_mmh3_x86_128 (test_input, i, GU_MMH32_SEED, &out);
        check (&test_output128[i], &out, sizeof(out));
    }
}
END_TEST
#endif /* 0 */

START_TEST (gu_mmh128_x64_test)
{
    int i;
    hash32_t out32;

    smhasher_verification (gu_mmh3_x64_128, sizeof(hash128_t), &out32);
    fail_if (check (&smhasher_checks[2], &out32, sizeof(out32)),
             "gu_mmh3_x64_128 failed.");

    for (i = 0; i < NUM_128_TESTS; i++)
    {
        hash128_t out;
        gu_mmh128 (test_input, i, &out);
        fail_if(check (&test_output128[i], &out, sizeof(out)),
                "gu_mmh128() failed at step %d", i);
    }
}
END_TEST

/* Tests partial hashing functions */
START_TEST (gu_mmh128_partial)
{
    hash128_t part;
    gu_mmh128_ctx_t ctx;
    gu_mmh128_init (&ctx);
    gu_mmh128_append (&ctx, test_input, 31);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[31], &part, sizeof(part)),
            "gu_mmh128_get() failed at one go");

    gu_mmh128_init (&ctx);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[0], &part, sizeof(part)),
            "gu_mmh128_get() failed at init");
    gu_mmh128_append (&ctx, test_input + 0, 0);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[0], &part, sizeof(part)),
            "gu_mmh128_get() failed at length %d", 0);
    gu_mmh128_append (&ctx, test_input + 0, 1);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[1], &part, sizeof(part)),
            "gu_mmh128_get() failed at length %d", 1);
    gu_mmh128_append (&ctx, test_input + 1, 2);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[3], &part, sizeof(part)),
            "gu_mmh128_get() failed at length %d", 3);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[3], &part, sizeof(part)),
            "gu_mmh128_get() failed at length %d again", 3);
    gu_mmh128_append (&ctx, test_input + 3, 20);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[23], &part, sizeof(part)),
            "gu_mmh128_get() failed at length %d", 23);
    gu_mmh128_append (&ctx, test_input + 23, 0);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[23], &part, sizeof(part)),
            "gu_mmh128_get() failed at length %d again", 23);
    gu_mmh128_append (&ctx, test_input + 23, 3);
    gu_mmh128_append (&ctx, test_input + 26, 3);
    gu_mmh128_append (&ctx, test_input + 29, 2);
    gu_mmh128_get (&ctx, &part);
    fail_if(check (&test_output128[31], &part, sizeof(part)),
            "gu_mmh128_get() failed at length %d", 31);
}
END_TEST

Suite *gu_mmh3_suite(void)
{
  Suite *s  = suite_create("MurmurHash3");
  TCase *tc = tcase_create("gu_mmh3");

  suite_add_tcase (s, tc);
  tcase_add_test (tc, gu_mmh32_test);
//  tcase_add_test (tc, gu_mmh128_x86_test);
  tcase_add_test (tc, gu_mmh128_x64_test);
  tcase_add_test (tc, gu_mmh128_partial);

  return s;
}

