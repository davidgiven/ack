#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pmap.h"

static void append(void* mapp, void* left, void* right)
{
    struct pmap* map = mapp;
    struct pmap_node* node;

	if (map->count == map->max)
	{
		int newmax = (map->max == 0) ? 8 : (map->max * 2);
		struct pmap_node* newarray = realloc(map->item, newmax * sizeof(*newarray));

		map->max = newmax;
		map->item = newarray;
	}

    node = &map->item[map->count];
    map->count++;

    node->left = left;
    node->right = right;
}

void pmap_put(void* mapp, void* left, void* right)
{
    struct pmap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct pmap_node* node = &map->item[i];
        if (node->left == left)
        {
            node->right = right;
            return;
        }
    }

    append(map, left, right);
}

void pmap_add(void* mapp, void* left, void* right)
{
    struct pmap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct pmap_node* node = &map->item[i];
        if ((node->left == left) && (node->right == right))
            return;
    }

    append(map, left, right);
}

void pmap_remove(void* mapp, void* left, void* right)
{
    struct pmap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
		struct pmap_node* node = &map->item[i];
		if ((node->left == left) && (node->right == right))
		{
			memmove(node, node+1, sizeof(*node) * (map->count - i - 1));
			map->count--;
			return;
		}
    }
}

void* pmap_findleft(void* mapp, void* left)
{
    struct pmap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct pmap_node* node = &map->item[i];
		if (node->left == left)
			return node->right;
    }

	return NULL;
}

void* pmap_findright(void* mapp, void* right)
{
    struct pmap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct pmap_node* node = &map->item[i];
		if (node->right == right)
			return node->left;
    }

	return NULL;
}


