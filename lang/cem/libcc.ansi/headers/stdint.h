/*
    <stdint.h> -- simple version used by "gimplify"

    last edit:  2007-03-02  D A Gwyn
*/

/* Although we try to unify integer types, some actual machine characteristics
   are visible, and these definitions need to work within their limits.  We do
   assume that the GCC target has an "int" at least 32 bits wide, which works
   for all our development platforms.  We use "long long" for the widest types
   because presumably the programmer really needed something special there. */

#define int8_t      signed char
#define uint8_t     unsigned char
#define int16_t     short
#define uint16_t    unsigned short
#define int32_t     int
#define uint32_t    unsigned
#define int64_t     long long
#define uint64_t    unsigned long long

#define int_least8_t    int
#define uint_least8_t   int
#define int_least16_t   int
#define uint_least16_t  unsigned
#define int_least32_t   int
#define uint_least32_t  unsigned
#define int_least64_t   long long
#define uint_least64_t  unsigned long long

#define intptr_t    int
#define uintptr_t   unsigned int

#define intmax_t    long long
#define uintmax_t   unsigned long long

#define int_fast8_t int
#define uint_fast8_t    unsigned int
#define int_fast16_t    int
#define uint_fast16_t   unsigned int
#define int_fast32_t    int
#define uint_fast32_t   unsigned int
#define int_fast64_t    long long int
#define uint_fast64_t   unsigned long long

#define INT8_MAX        127
#define INT8_MIN        (-128)
#define UINT8_MAX       255

#define INT16_MAX       32767
#define INT16_MIN       (-32768)
#define UINT16_MAX      65535

#define INT32_MAX       2147483647
#define INT32_MIN       (-2147483648)
#define UINT32_MAX      4294967295

#define INT64_MAX       2147483647LL
#define INT64_MIN       (-2147483648LL)
#define UINT64_MAX      4294967295ULL

#define INT_LEAST8_MAX      2147483647
#define INT_LEAST8_MIN      (-2147483647)
#define UINT_LEAST8_MAX     4294967295

#define INT_LEAST16_MAX     2147483647
#define INT_LEAST16_MIN     (-2147483647)
#define UINT_LEAST16_MAX    4294967295

#define INT_LEAST32_MAX     2147483647
#define INT_LEAST32_MIN     (-2147483647)
#define UINT_LEAST32_MAX    4294967295

#define INT_LEAST64_MAX     2147483647LL
#define INT_LEAST64_MIN     (-2147483647LL)
#define UINT_LEAST64_MAX    4294967295ULL

#define INT_FAST8_MAX       2147483647
#define INT_FAST8_MIN       (-2147483647)
#define UINT_FAST8_MAX      4294967295

#define INT_FAST16_MAX      2147483647
#define INT_FAST16_MIN      (-2147483647)
#define UINT_FAST16_MAX     4294967295

#define INT_FAST32_MAX      2147483647
#define INT_FAST32_MIN      (-2147483647)
#define UINT_FAST32_MAX     4294967295

#define INT_FAST64_MAX      2147483647LL
#define INT_FAST64_MIN      (-2147483647LL)
#define UINT_FAST64_MAX     4294967295ULL

#define INTPTR_MAX      2147483647
#define INTPTR_MIN      (-2147483647)
#define UINTPTR_MAX     4294967295

#define INTMAX_MAX      2147483647LL
#define INTMAX_MIN      (-2147483647LL)
#define UINTMAX_MAX     4294967295ULL

#define PTRDIFF_MAX     2147483647
#define PTRDIFF_MIN     (-2147483647)

#define SIG_ATOMIC_MAX      4294967295
#define SIG_ATOMIC_MIN      0

#define SIZE_MAX        4294967295

/* The trick used to get the right type is due to Clive Feather: */
#define INT8_C(c)   (INT_LEAST8_MAX - INT_LEAST8_MAX + (c))
#define UINT8_C(c)  (UINT_LEAST8_MAX - UINT_LEAST8_MAX + (c))
#define INT16_C(c)  (INT_LEAST16_MAX - INT_LEAST16_MAX + (c))
#define UINT16_C(c) (UINT_LEAST16_MAX - UINT_LEAST16_MAX + (c))
#define INT32_C(c)  (INT_LEAST32_MAX - INT_LEAST32_MAX + (c))
#define UINT32_C(c) (UINT_LEAST32_MAX - UINT_LEAST32_MAX + (c))
#define INT64_C(c)  (INT_LEAST64_MAX - INT_LEAST64_MAX + (c))
#define UINT64_C(c) (UINT_LEAST64_MAX - UINT_LEAST64_MAX + (c))
#define INTMAX_C(c) (INTMAX_MAX - INTMAX_MAX + (c))
#define UINTMAX_C(c)    (UINTMAX_MAX - UINTMAX_MAX + (c))
