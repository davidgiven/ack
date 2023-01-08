/** @file
 * String map implementation.
 *
 */

#ifndef SMAP_H
#define SMAP_H

/* A string multimap (each key can have multiple values). */

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

/** Initializes a string map. Optional; a zero-initialised map
 * structure is a valid empty map.
 */
extern void smap_init(void *mapp);

/** Adds a new item with name `key` in the string map if it does not already
 * exist. Otherwise replaces the value `value` associated with the existing
 * `key`.
 *
 * Use this for normal map mode.
 */
extern void smap_put(void *mapp, const char* key, void* value);

/** Adds a new item with name `key` and value `value`. If that relation already
 * exists, nothing happens.
 *
 * Use this for multimap mode.
 */
extern void smap_add(void *mapp, const char* key, void* value);

/** Returns the first value associated with the specified `key`. Returns NULL
 * if `key` is not present in the string map.
 *
 * Use this in normal map mode.
 */
extern void* smap_get(void* mapp, const char* key);

/** Frees the data structure associated with the string map.
 *  Also frees the memory associated with the key if
 *  `free_key` is non-zero, and free the memory associated
 *  with the value if `free_value` is non-zero.
 *
 */
extern void smap_free(void *mapp, int free_key, int free_value);

#endif

