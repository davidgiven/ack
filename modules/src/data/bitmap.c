#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <limits.h>
#include <assert.h>
#include "bitmap.h"

unsigned int* bitmap_alloc(int size)
{
	return calloc(WORDS_FOR_BITMAP_SIZE(size), sizeof(unsigned int));
}

void bitmap_set(unsigned int* bitmap, int size, int bit)
{
	int word = bit / BITS_PER_WORD;
	bit = bit & (BITS_PER_WORD - 1);

	assert((bit >= 0) && (bit < size));
	bitmap[word] |= 1<<bit;
}

bool bitmap_get(unsigned int* bitmap, int size, int bit)
{
	int word = bit / BITS_PER_WORD;
	bit = bit & (BITS_PER_WORD - 1);

	assert((bit >= 0) && (bit < size));
	return bitmap[word] & (1<<bit);
}

int bitmap_findfirst(unsigned int* bitmap, int size)
{
	int word;

	for (word=0; word<WORDS_FOR_BITMAP_SIZE(size); word++)
	{
		unsigned int w = bitmap[word];
		if (w)
			return (ffs(w)-1) + word*BITS_PER_WORD;
	}

	return -1;
}

void bitmap_and(unsigned int* dest, int size, unsigned int* src)
{
	int word;
	int words = ((size-1) / BITS_PER_WORD) + 1;

	for (word=0; word<words; word++)
		dest[word] &= src[word];
}

void bitmap_or(unsigned int* dest, int size, unsigned int* src)
{
	int word;
	int words = ((size-1) / BITS_PER_WORD) + 1;

	for (word=0; word<words; word++)
		dest[word] |= src[word];
}

int bitmap_find_unset_bit(unsigned int* bitmap, int size)
{
	int word;
	int words = ((size-1) / BITS_PER_WORD) + 1;

	for (word=0; word<words; word++)
	{
		unsigned int w = bitmap[word];
		if (w != UINT_MAX)
		{
			int mask = 1;
			int pos = 0;

			while (w & mask)
			{
				mask <<= 1;
				pos++;
			}

			return (word*BITS_PER_WORD) + pos;
		}
	}

	return -1;
}
