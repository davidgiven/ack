#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "set.h"

void set_empty(struct set* s)
{
	hashtable_empty(&s->table);
}

void set_reset(struct set* s)
{
	hashtable_reset(&s->table);
}

bool set_add(struct set* s, void* item)
{
	return hashtable_put(&s->table, item, item);
}

void* set_remove(struct set* s, void* item)
{
	return hashtable_remove(&s->table, item);
}

void* set_get(struct set* s, void* item)
{
	return hashtable_get(&s->table, item);
}

void* set_pop(struct set* s)
{
	return hashtable_pop(&s->table);
}

void* set_next(struct set* s, struct set_iterator* sit)
{
	sit->item = hashtable_next(&s->table, &sit->hit);
	return sit->item;
}

