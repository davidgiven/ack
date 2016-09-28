#ifndef ARRAY_H
#define ARRAY_H

/* Danger, Will Robinson! The type and the macro must be compatible. */

struct array
{
	void** item;
	int count;
	int max;
};

#define ARRAYOF(TYPE) \
	struct { \
		TYPE** item; \
		int count; \
		int max; \
	}

extern void array_append(void* array, void* value);
extern bool array_appendu(void* array, void* value);
extern void array_remove(void* array, void* value);
extern bool array_contains(void* array, void* value);

#endif

