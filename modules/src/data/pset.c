#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "array.h"
#include "pset.h"

void pset_add(void* set, void* value)
{
	assert(value);
	array_appendu(set, value);
}

void pset_remove(void* set, void* value)
{
    struct array* array = set;
    int i;

    for (i=0; i<array->count; i++)
    {
        if (array->item[i] == value)
        {
			array->item[i] = array->item[array->count-1];
            array->count--;
            return;
        }
    }
}

bool pset_contains(void* set, void* value)
{
	return array_contains(set, value);
}

void* pset_pop(void* set)
{
	return array_pop(set);
}

