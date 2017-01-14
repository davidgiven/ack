#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "hashtable.h"

struct hashnode
{
    struct hashnode* next;
    void* key;
    void* value;
};

uint32_t standard_pointer_hash_function(void* key)
{
    /* Basile-Starynkevitch pointer hash */
    uintptr_t ptr = (uintptr_t) key;
    return (uint32_t) ((13 * ptr) ^ (ptr >> 15));
}

bool standard_pointer_comparison_function(void* key1, void* key2)
{
    return (key1 == key2);
}

uint32_t standard_string_hash_function(void* key)
{
	char* s = key;
	uint32_t hash = 0;

	while (*s)
	{
		hash = ((hash << 5) + hash) ^ *s;
		s++;
	}

	return hash;
}

bool standard_string_comparison_function(void* key1, void* key2)
{
	return strcmp(key1, key2) == 0;
}

static void lazy_init(struct hashtable* ht)
{
    if (!ht->num_buckets)
        ht->num_buckets = 7;

    if (!ht->hashfunction)
        ht->hashfunction = standard_pointer_hash_function;

    if (!ht->cmpfunction)
        ht->cmpfunction = standard_pointer_comparison_function;

    if (!ht->allocfunction)
        ht->allocfunction = calloc;

    if (!ht->freefunction)
        ht->freefunction = free;

    if (!ht->buckets)
        ht->buckets = ht->allocfunction(ht->num_buckets, sizeof(struct hashnode*));
}

void hashtable_empty(struct hashtable* ht)
{
    while (ht->size)
        hashtable_pop(ht);
}

void hashtable_reset(struct hashtable* ht)
{
    lazy_init(ht);
	hashtable_empty(ht);
    ht->freefunction(ht->buckets);
    ht->buckets = NULL;
}

void hashtable_rebucket(struct hashtable* ht, unsigned int num_buckets)
{
    struct hashnode** old_buckets = ht->buckets;
	int old_num_buckets = ht->num_buckets;
	int i;

	ht->num_buckets = num_buckets;
	ht->buckets = ht->allocfunction(num_buckets, sizeof(struct hashnode*));

	for (i=0; i<old_num_buckets; i++)
	{
		while (old_buckets[i])
		{
			struct hashnode* hn = old_buckets[i];
			uint32_t hash = ht->hashfunction(hn->key) & (num_buckets-1);

			old_buckets[i] = hn->next;
			hn->next = ht->buckets[hash];
			ht->buckets[hash] = hn;
		}
	}

	ht->freefunction(old_buckets);
}

static struct hashnode** findnodep(struct hashtable* ht, void* key)
{
    uint32_t hash;
    struct hashnode** hnp;

    lazy_init(ht);
    hash = ht->hashfunction(key) & (ht->num_buckets-1);
    hnp = &ht->buckets[hash];

	for (;;)
	{
		void* k;

		if (!*hnp)
			break;
		k = (*hnp)->key;
		if ((k == key) || ht->cmpfunction(k, key))
			break;

        hnp = &(*hnp)->next;
	}

    return hnp;
}

void* hashtable_put(struct hashtable* ht, void* key, void* value)
{
    void* oldvalue;
    struct hashnode** hnp = findnodep(ht, key);
    if (!*hnp)
    {
		if (ht->size == (ht->num_buckets*2))
		{
			hashtable_rebucket(ht, ht->num_buckets*4);
			return hashtable_put(ht, key, value);
		}

        *hnp = ht->allocfunction(1, sizeof(struct hashnode));
        ht->size++;
    }

    oldvalue = (*hnp)->value;
    (*hnp)->key = key;
    (*hnp)->value = value;
    return oldvalue;
}

void* hashtable_get(struct hashtable* ht, void* key)
{
    if (ht)
    {
        struct hashnode** hnp = findnodep(ht, key);
        if (*hnp)
            return (*hnp)->value;
    }
    return NULL;
}

void* hashtable_remove(struct hashtable* ht, void* key)
{
    struct hashnode** hnp = findnodep(ht, key);
    if (*hnp)
    {
        struct hashnode* hn = *hnp;
        void* value = hn->value;
        *hnp = hn->next;
        ht->freefunction(hn);
        ht->size--;
        return value;
    }

    return NULL;
}

void* hashtable_pop(struct hashtable* ht)
{
    int i;

    if (!ht || (ht->size == 0))
        return NULL;

    lazy_init(ht);
    for (i=0; i<ht->num_buckets; i++)
    {
        struct hashnode** hnp = &ht->buckets[i];
        if (*hnp)
        {
            struct hashnode* hn = *hnp;
            void* value = hn->value;
            *hnp = hn->next;
            ht->freefunction(hn);
            ht->size--;
            return value;
        }
    }

    return NULL;
}

void* hashtable_next(struct hashtable* ht, struct hashtable_iterator* it)
{
    if (!ht)
        return NULL;

	lazy_init(ht);

    if (!it->running)
    {
        /* Find the first node and return it; the stored state in the iterator
         * is invalid. */

         while (it->bucket < ht->num_buckets)
         {
             it->node = ht->buckets[it->bucket];
             if (it->node)
                goto found;
            it->bucket++;
         }

         /* hashtable is empty! */
    }
    else
    {
        /* The iterator is running; the stored state in the iterator points at
         * the current node, so find the next one. */

        if (!it->advanced)
            it->node = it->node->next;
        if (it->node)
            goto found;

        for (;;)
        {
            it->bucket++;
            if (it->bucket == ht->num_buckets)
                break;

            it->node = ht->buckets[it->bucket];
            if (it->node)
                goto found;
        }

        /* nothing left found! */
    }

    /* EOF: reset the iterator. */

    it->running = false;
    it->advanced = false;
	it->key = it->value = NULL;
	it->bucket = 0;
	it->node = NULL;
	return NULL;

found:
    it->running = true;
    it->advanced = false;
    it->key = it->node->key;
	it->value = it->node->value;
    return it->value;
}

void hashtable_delete_current(struct hashtable* ht, struct hashtable_iterator* it)
{
    struct hashnode** hnp;

    assert(ht);
    assert(it->running);

    hnp = findnodep(ht, it->node->key);
    it->node = it->node->next;
    ht->freefunction(*hnp);
    ht->size--;
    *hnp = it->node;

    it->advanced = true;
}

void hashtable_copy_all(struct hashtable* src, struct hashtable* dest)
{
    struct hashtable_iterator hit = {};
    while (hashtable_next(src, &hit))
        hashtable_put(dest, hit.key, hit.value);
}
