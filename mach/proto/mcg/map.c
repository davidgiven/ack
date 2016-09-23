#include "mcg.h"

static void extend(struct map_node** map, int* count, int* max)
{
	if (*count == *max)
	{
		int newmax = (*max == 0) ? 8 : (*max * 2);
		struct map_node* newmap = realloc(*map, newmax * sizeof(struct map_node));
		if (!newmap)
			fatal("memory allocation failure");

		*max = newmax;
		*map = newmap;
	}
}

void map_set(struct map_node** map, int* count, int* max, void* left, void* right)
{
	int i;
	struct map_node* node;

	for (i=0; i<*count; i++)
	{
		node = &(*map)[i];
		if (node->left == left)
		{
			node->right = right;
			return;
		}
	}

	extend(map, count, max);
	node = &(*map)[*count];
	node->left = left;
	node->right = right;
	(*count)++;
}

void map_add(struct map_node** map, int* count, int* max, void* left, void* right)
{
	int i;
	struct map_node* node;

	for (i=0; i<*count; i++)
	{
		node = &(*map)[i];
		if ((node->left == left) && (node->right == right))
			return;
	}

	extend(map, count, max);
	node = &(*map)[*count];
	node->left = left;
	node->right = right;
	(*count)++;
}

/* vim: set sw=4 ts=4 expandtab : */
