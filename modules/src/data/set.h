#ifndef SET_H
#define SET_H

/* A hashtable-based identity set. */

#include "hashtable.h"

struct set
{
	struct hashtable table;
};

#define SET_OF_STRINGS \
	{ HASHTABLE_OF_STRINGS }

extern void set_empty(struct set* s);
extern void set_reset(struct set* s);

extern bool set_add(struct set* s, void* item);
extern void* set_remove(struct set* s, void* item);
extern void* set_get(struct set* s, void* item);
extern void* set_get_any(struct set* s);
extern void* set_pop(struct set* s);

struct set_iterator
{
	/* Public */
	void* item;

	/* Private */
	struct hashtable_iterator hit;
};

extern void* set_next(struct set* s, struct set_iterator* sit);

#endif

