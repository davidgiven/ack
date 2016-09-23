#ifndef MAP_H
#define MAP_H

struct map_node
{
	void* left;
	void* right;
};

#define _MAP(MODIFIER, NAME) \
	MODIFIER struct map_node* NAME; \
	MODIFIER int NAME##_count; \
	MODIFIER int NAME##_max

#define MAP(NAME) _MAP(, NAME)
#define STATICMAP(NAME) _MAP(static, NAME)

#define MAP_SET(MAP, LEFT, RIGHT) \
	map_set(&MAP, &MAP##_count, &MAP##_max, LEFT, RIGHT)

extern void map_set(struct map_node** map, int* count, int* max, void* left, void* right);
extern void map_add(struct map_node** map, int* count, int* max, void* left, void* right);

#endif


