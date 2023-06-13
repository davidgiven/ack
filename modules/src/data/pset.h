#ifndef PSET_H
#define PSET_H

/* This is a wrapper around an array that implements a set. */

#define PSETOF(t) ARRAYOF(t)

/* Adds an item to the set. Does nothing if it's already there.
 * WARNING: using while iterating may cause elements to be skipped (so don't).
 */

extern void pset_add(void* set, void* value);

/* Removes an item from the set. Does nothing if it's not there.
 * WARNING: using while iterating may cause elements to be skipped (so don't).
 */

extern void pset_remove(void* set, void* value);

/* Tests if the array contains an item. */

extern bool pset_contains(void* set, void* value);

/* Removes and returns an arbitrary item from the set.
 * WARNING: using while iterating may cause elements to be skipped (so don't).
 */

extern void* pset_pop(void* set);

#endif

