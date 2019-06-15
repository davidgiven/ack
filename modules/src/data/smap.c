#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "smap.h"

#define INCR_SIZE	8

static void append(void* mapp, char* key, void* value)
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


void smap_put(struct smap *mapp, char* key, void* value)
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

void smap_init(struct smap *mapp)
{
	mapp->count = 0;
	mapp->item = NULL;
	mapp->max = 0;
}


void smap_free(struct smap *mapp, int free_key, int free_value)
{
	int i;
    for (i=0; i<mapp->count; i++)
    {
        struct smap_node* node = &mapp->item[i];
        if (free_key)
        {
        	free(node->left);
        }
        if (free_value)
        {
        	free(node->right);
        }
    }
	mapp->count = 0;
	free(mapp->item);
}

void smap_add(struct smap *mapp, char* key, void* value)
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

void* smap_get(struct smap *mapp, const char* left)
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

