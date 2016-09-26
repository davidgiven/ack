#ifndef MAP_H
#define MAP_H

struct map_node
{
	void* left;
	void* right;
};

/* Danger, Will Robinson! The type and the macro must be compatible. */

struct map
{
	struct map_node* item;
	int count;
	int max;
};

#define MAPOF(LEFT, RIGHT) \
	struct { \
		struct { LEFT* left; RIGHT* right; }* item; \
		int count; \
		int max; \
	}

extern void map_putp(void* map, void* left, void* right);
extern void map_addp(void* map, void* left, void* right);

#endif

