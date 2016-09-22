#include "mcg.h"
#include "array.h"

void array_append(void*** array, int* count, int* max, void* value)
{
	if (*count == *max)
	{
		int newmax = (*max == 0) ? 8 : (*max * 2);
		void** newarray = realloc(*array, newmax * sizeof(void*));
		if (!newarray)
			fatal("memory allocation failure");

		*max = newmax;
		*array = newarray;
	}

	(*array)[*count] = value;
	(*count)++;
}

bool array_contains(void** array, int count, void* value)
{
	int i;

	for (i=0; i<count; i++)
		if (array[i] == value)
			return true;

	return false;
}

void array_appendu(void*** array, int* count, int* max, void* value)
{
	if (!array_contains(*array, *count, value))
		array_append(array, count, max, value);
}

void array_remove(void** array, int* count, void* value)
{
    int i;

    for (i=0; i<*count; i++)
    {
        if (array[i] == value)
        {
            while (i < (*count-1))
            {
                array[i] = array[i+1];
                i++;
            }
            (*count)--;
            return;
        }
    }
}

/* vim: set sw=4 ts=4 expandtab : */

