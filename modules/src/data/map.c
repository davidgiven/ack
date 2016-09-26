#include <stdlib.h>
#include <stdbool.h>
#include "map.h"

static void append(void* mapp, void* left, void* right)
{
    struct map* map = mapp;
    struct map_node* node;

	if (map->count == map->max)
	{
		int newmax = (map->max == 0) ? 8 : (map->max * 2);
		struct map_node* newarray = realloc(map->item, newmax * sizeof(*newarray));

		map->max = newmax;
		map->item = newarray;
	}

    node = &map->item[map->count];
    map->count++;

    node->left = left;
    node->right = right;
}

void map_putp(void* mapp, void* left, void* right)
{
    struct map* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct map_node* node = &map->item[i];
        if (node->left == left)
        {
            node->right = right;
            return;
        }
    }

    append(map, left, right);
}

void map_addp(void* mapp, void* left, void* right)
{
    struct map* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct map_node* node = &map->item[i];
        if ((node->left == left) && (node->right == right))
            return;
    }

    append(map, left, right);
}

