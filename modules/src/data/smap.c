#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "smap.h"

#define INCR_SIZE	8

static void append(void* mapp, const char* key, void* value)
{
    struct smap* map = mapp;
    struct smap_node* node;
	if (map->count == map->max)
	{
		int newmax = (map->max == 0) ? INCR_SIZE : (map->max * 2);
		struct smap_node* newarray = realloc(map->item, newmax * sizeof(*newarray));

		map->max = newmax;
		map->item = newarray;
	}

    node = &map->item[map->count];
    map->count++;

    node->left = key;
    node->right = value;
}


void smap_put(void *mapp, const char* key, void* value)
{
    struct smap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct smap_node* node = &map->item[i];
        if (strcmp(node->left, key) == 0)
        {
            node->right = value;
            return;
        }
    }

    append(map, key, value);
}

void smap_init(void *mapp)
{
    struct smap* map = mapp;
	map->count = 0;
	map->item = NULL;
	map->max = 0;
}


void smap_free(void *mapp, int free_key, int free_value)
{
    struct smap* map = mapp;
	int i;
    for (i=0; i<map->count; i++)
    {
        struct smap_node* node = &map->item[i];
        if (free_key)
        {
        	free((void*) node->left);
        }
        if (free_value)
        {
        	free(node->right);
        }
    }
	map->count = 0;
	free(map->item);
}

void smap_add(void *mapp, const char* key, void* value)
{
    struct smap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct smap_node* node = &map->item[i];
        if ((strcmp(node->left, key) == 0) && (node->right == value))
            return;
    }

    append(map, key, value);
}

void* smap_get(void *mapp, const char* left)
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

