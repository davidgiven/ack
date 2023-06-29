#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "array.h"

static void reserve(struct array* array, size_t max)
{
	void** newitem;

	if (max <= array->max)
		return;

	int newmax = (array->max == 0) ? 8 : (array->max * 2);
	if (newmax < max)
		newmax = max;

	newitem = realloc(array->item, newmax * sizeof(void*));
	memset(newitem + array->count, 0, (newmax - array->count) * sizeof(void*));

	array->max = newmax;
	array->item = (void**)newitem;
}

void array_resize(void* arrayp, size_t size)
{
	struct array* array = arrayp;

	reserve(array, size);
	array->count = size;
}

void array_append(void* arrayp, void* value)
{
	struct array* array = arrayp;

	reserve(array, array->count + 1);
	array->item[array->count] = value;
	array->count++;
}

int array_indexof(void* arrayp, void* value)
{
	struct array* array = arrayp;
	int i;

	for (i = 0; i < array->count; i++)
		if (array->item[i] == value)
			return i;

	return -1;
}

bool array_contains(void* arrayp, void* value)
{
	return (array_indexof(arrayp, value) != -1);
}

bool array_appendu(void* arrayp, void* value)
{
	if (array_contains(arrayp, value))
		return true;

	array_append(arrayp, value);
	return false;
}

void array_insert(void* arrayp, void* value, int before)
{
	struct array* array = arrayp;

	reserve(array, array->count + 1);
	memmove(
	    &array->item[before + 1], &array->item[before],
	    (array->count - before) * sizeof(void*));
	array->item[before] = value;
	array->count++;
}

void array_remove(void* arrayp, void* value)
{
	struct array* array = arrayp;
	int i;

	for (i = 0; i < array->count; i++)
	{
		if (array->item[i] == value)
		{
			while (i < (array->count - 1))
			{
				array->item[i] = array->item[i + 1];
				i++;
			}
			array->count--;
			return;
		}
	}
}

void* array_pop(void* arrayp)
{
	struct array* array = arrayp;

	assert(array->count > 0);
	return array->item[--array->count];
}

void array_appendall(void* arrayp, void* srcp)
{
	struct array* array = arrayp;
	struct array* src = srcp;
	int i;

	for (i = 0; i < src->count; i++)
		array_append(array, src->item[i]);
}

void array_removeall(void* arrayp, void* srcp)
{
	struct array* array = arrayp;
	struct array* src = srcp;
	int i;

	for (i = 0; i < src->count; i++)
		array_remove(array, src->item[i]);
}

bool array_appendallu(void* arrayp, void* srcp)
{
	struct array* array = arrayp;
	struct array* src = srcp;
	bool unchanged = true;
	int i;

	for (i = 0; i < src->count; i++)
		unchanged &= array_appendu(array, src->item[i]);

	return unchanged;
}

void array_sort(void* arrayp, int (*cb)(const void*, const void*))
{
	struct array* array = arrayp;
	qsort(array->item, array->count, sizeof(void*), cb);
}

/* vim: set sw=4 ts=4 expandtab : */
