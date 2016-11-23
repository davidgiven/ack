#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "smap.h"

static void append(void* mapp, const char* left, void* right)
{
    struct smap* map = mapp;
    struct smap_node* node;

	if (map->count == map->max)
	{
		int newmax = (map->max == 0) ? 8 : (map->max * 2);
		struct smap_node* newarray = realloc(map->item, newmax * sizeof(*newarray));

		map->max = newmax;
		map->item = newarray;
	}

    node = &map->item[map->count];
    map->count++;

    node->left = left;
    node->right = right;
}

void smap_put(void* mapp, const char* left, void* right)
{
    struct smap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct smap_node* node = &map->item[i];
        if (strcmp(node->left, left) == 0)
        {
            node->right = right;
            return;
        }
    }

    append(map, left, right);
}

void smap_add(void* mapp, const char* left, void* right)
{
    struct smap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct smap_node* node = &map->item[i];
        if ((strcmp(node->left, left) == 0) && (node->right == right))
            return;
    }

    append(map, left, right);
}

void* smap_get(void* mapp, const char* left)
{
    struct smap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct smap_node* node = &map->item[i];
		if (strcmp(node->left, left) == 0)
			return node->right;
    }

	return NULL;
}

