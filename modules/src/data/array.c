#include <stdlib.h>
#include <stdbool.h>
#include "array.h"

void array_append(void* arrayp, void* value)
{
    struct array* array = arrayp;

	if (array->count == array->max)
	{
		int newmax = (array->max == 0) ? 8 : (array->max * 2);
		void** newarray = realloc(array->item, newmax * sizeof(void*));

		array->max = newmax;
		array->item = newarray;
	}

    array->item[array->count] = value;
    array->count++;
}

bool array_contains(void* arrayp, void* value)
{
    struct array* array = arrayp;
	int i;

	for (i=0; i<array->count; i++)
		if (array->item[i] == value)
			return true;

	return false;
}

void array_appendu(void* arrayp, void* value)
{
	if (!array_contains(arrayp, value))
		array_append(arrayp, value);
}

void array_remove(void* arrayp, void* value)
{
    struct array* array = arrayp;
    int i;

    for (i=0; i<array->count; i++)
    {
        if (array->item[i] == value)
        {
            while (i < (array->count-1))
            {
                array->item[i] = array->item[i+1];
                i++;
            }
            array->count--;
            return;
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */

