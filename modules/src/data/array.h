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
extern void array_insert(void* array, void* value, int before);
extern void array_remove(void* array, void* value);
extern bool array_contains(void* array, void* value);
extern int array_indexof(void* array, void* value);

#define array_push(a, v) array_append(a, v)
extern void* array_pop(void* array);

#endif

