#ifndef BSET_H
#define BSET_H

struct bitset
{
	size_t bytes;
	size_t bits;
	uint8_t* data;
};

extern void bitset_resize(struct bitset* bitset, size_t size);
extern void bitset_clear(struct bitset* bitset);
extern void bitset_set(struct bitset* bitset, size_t index, bool value);
extern bool bitset_get(struct bitset* bitset, size_t index);

#endif

