#include <stdlib.h>
#include <stdbool.h>
#include "imap.h"

static void append(void* mapp, int left, void* right)
{
    struct imap* map = mapp;
    struct imap_node* node;

	if (map->count == map->max)
	{
		int newmax = (map->max == 0) ? 8 : (map->max * 2);
		struct imap_node* newarray = realloc(map->item, newmax * sizeof(*newarray));

		map->max = newmax;
		map->item = newarray;
	}

    node = &map->item[map->count];
    map->count++;

    node->left = left;
    node->right = right;
}

void imap_put(void* mapp, int left, void* right)
{
    struct imap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct imap_node* node = &map->item[i];
        if (node->left == left)
        {
            node->right = right;
            return;
        }
    }

    append(map, left, right);
}

void imap_add(void* mapp, int left, void* right)
{
    struct imap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct imap_node* node = &map->item[i];
        if ((node->left == left) && (node->right == right))
            return;
    }

    append(map, left, right);
}

void* imap_get(void* mapp, int left)
{
    struct imap* map = mapp;
    int i;

    for (i=0; i<map->count; i++)
    {
        struct imap_node* node = &map->item[i];
		if (node->left == left)
			return node->right;
    }

	return NULL;
}


