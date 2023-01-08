#ifndef PMAP_H
#define PMAP_H

/* A bidirectional multimap, storing (left, right) tuples.
 */

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

/** Adds a new relation `(left -> right)` to the map
 * if `left` is not present. Otherwise replaces the
 * `right` associated with the first `left`.
 *
 * Use this in normal map mode.
 */
extern void pmap_put(void* map, void* left, void* right);

/** Adds a new relation to the map if that relation does
 * not already exist.
 *
 * Use this in multimap mode.
 */
extern void pmap_add(void* map, void* left, void* right);

/** Removes a relation from the map.
 *
 * Use this in multimap mode.
 */
extern void pmap_remove(void* map, void* left, void* right);

/** Given a `left`, find the first matching `right`.
 */
extern void* pmap_findleft(void* map, void* left);

/** Given a `right`, find the first matching `left`.
 */
extern void* pmap_findright(void* map, void* right);

#endif

