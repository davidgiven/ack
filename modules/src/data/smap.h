/** @file
 * String map implementation.
 *
 */

#ifndef SMAP_H
#define SMAP_H

struct smap_node
{
	char* left;
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
		struct { char* left; RIGHT* right; }* item; \
		int count; \
		int max; \
	}

/** Initializes a string map and returns the initialized
 *  handle in `mapp`.
 */
extern void smap_init(struct smap *mapp);
/** Adds a new item with name `key` in the string map if
 *  it does not already exist, otherwise replaces the
 *  value `value` associated with the existing `key`.
 */
extern void smap_put(struct smap *mapp, char* key, void* value);
/** Adds a new item in a string map only if `key` does
 *  not already exist in the string map.
 */
extern void smap_add(struct smap *mapp, char* key, void* value);
/** Returns the value associated with the specified `key`, returns
 *  NULL if `key` is not present in the string map.
 *
 */
extern void* smap_get(struct smap* mapp, const char* key);
/** Frees the data structure associated with the string map.
 *  Also frees the memory associated with the key if
 *  `free_key` is non-zero, and free the memory associated
 *  with the value if `free_value` is non-zero.
 *
 */
extern void smap_free(struct smap *mapp, int free_key, int free_value);

#endif

