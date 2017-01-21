#ifndef BITMAP_H
#define BITMAP_H

extern unsigned int* bitmap_alloc(int size);

extern void bitmap_set(unsigned int* bitmap, int size, int bit);
extern bool bitmap_get(unsigned int* bitmap, int size, int bit);
extern int bitmap_findfirst(unsigned int* bitmap, int size);
extern void bitmap_and(unsigned int* dest, int size, unsigned int* src);
extern void bitmap_or(unsigned int* dest, int size, unsigned int* src);
extern int bitmap_find_unset_bit(unsigned int* bitmap, int size);

#define BITS_PER_WORD (sizeof(unsigned int)*8)
#define WORDS_FOR_BITMAP_SIZE(n) (((n-1) / BITS_PER_WORD) + 1)

#endif

