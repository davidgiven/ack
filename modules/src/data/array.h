#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY(TYPE, NAME) \
	TYPE** NAME; \
	int NAME##_count; \
	int NAME##_max

#define STATICARRAY(TYPE, NAME) \
	static TYPE** NAME; \
	static int NAME##_count; \
	static int NAME##_max

#define APPEND(ARRAY, VALUE) \
	array_append((void***) &ARRAY, &ARRAY##_count, &ARRAY##_max, VALUE)

#define CONTAINS(ARRAY, VALUE) \
	array_contains((void**) ARRAY, ARRAY##_count, VALUE)

#define APPENDU(ARRAY, VALUE) \
	array_appendu((void***) &ARRAY, &ARRAY##_count, &ARRAY##_max, VALUE)

#define REMOVE(ARRAY, VALUE) \
	array_remove((void**) ARRAY, &ARRAY##_count, VALUE)

extern void array_append(void*** array, int* count, int* max, void* value);
extern bool array_contains(void** array, int count, void* value);
extern void array_appendu(void*** array, int* count, int* max, void* value);
extern void array_remove(void** array, int* count, void* value);

#endif

