#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "bset.h"

void bitset_resize(struct bitset* bitset, size_t size)
{
	bitset->bits = size;
	bitset->bytes = (size + 7) / 8;
	bitset->data = realloc(bitset->data, bitset->bytes);
	bitset_clear(bitset);
}

void bitset_clear(struct bitset* bitset)
{
	memset(bitset->data, 0, bitset->bytes);
}

void bitset_set(struct bitset* bitset, size_t index, bool value)
{
	uint8_t* b;
	uint8_t bitfield;

	assert(index < bitset->bits);
	bitfield = ~(1 << (index % 8));
	b = &bitset->data[index / 8];

	if (value)
		*b |= bitfield;
	else
		*b &= ~bitfield;
}

bool bitset_get(struct bitset* bitset, size_t index)
{
	uint8_t* b;
	uint8_t bitfield;

	assert(index < bitset->bits);
	bitfield = ~(1 << (index % 8));
	b = &bitset->data[index / 8];

	return *b & bitfield;
}
