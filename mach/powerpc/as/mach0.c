/*
 * $Source$
 * $State$
 */

#define	THREE_PASS          /* branch and offset optimization */
#define BYTES_REVERSED      /* high order byte has lowest address */
#define WORDS_REVERSED      /* high order word has lowest address */
#define LISTING             /* enable listing facilities */
#define RELOCATION          /* generate relocatable code */
#define DEBUG 0

#undef ADDR_T
#define ADDR_T uint32_t

#undef word_t
#define word_t uint32_t

#undef ALIGNWORD
#define ALIGNWORD	4

#undef ALIGNSECT
#define ALIGNSECT	4

#undef VALWIDTH
#define VALWIDTH	8

#define FIXUPFLAGS (RELBR | RELWR)

/* 6-bit mb (mask begin) or me (mask end) field */
#define MB6(v) (((v) & 0x1F)<<6 | ((v) & 0x20)>>0)

/* 6-bit sh (shift) field */
#define SH6(v) (((v) & 0x1F)<<11 | ((v) & 0x20)>>4)
