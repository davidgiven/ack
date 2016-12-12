#ifndef PMAP_H
#define PMAP_H

struct pmap_node
{
	void* left;
	void* right;
};

/* Danger, Will Robinson! The type and the macro must be compatible. */

struct pmap
{
	struct pmap_node* item;
	int count;
	int max;
};

#define PMAPOF(LEFT, RIGHT) \
	struct { \
		struct { LEFT* left; RIGHT* right; }* item; \
		int count; \
		int max; \
	}

extern void pmap_put(void* map, void* left, void* right);
extern void pmap_add(void* map, void* left, void* right);
extern void pmap_add_bi(void* map, void* left, void* right);
extern void pmap_remove(void* map, void* left, void* right);
extern void pmap_remove_bi(void* map, void* left, void* right);
extern void pmap_remove_either(void* map, void* either);
extern bool pmap_contains_bi(void* map, void* left, void* right);
extern void* pmap_findleft(void* map, void* left);
extern void* pmap_findright(void* map, void* right);

#endif

