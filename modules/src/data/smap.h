#ifndef SMAP_H
#define SMAP_H

struct smap_node
{
	const char* left;
	void* right;
};

/* Danger, Will Robinson! The type and the macro must be compatible. */

struct smap
{
	struct smap_node* item;
	int count;
	int max;
};

#define SMAPOF(RIGHT) \
	struct { \
		struct { const char* left; RIGHT* right; }* item; \
		int count; \
		int max; \
	}

extern void smap_put(void* map, const char* left, void* right);
extern void smap_add(void* map, const char* left, void* right);
extern void* smap_get(void* map, const char* left);

#endif

