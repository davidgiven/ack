#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "hashtable.h"

struct hashnode
{
    struct hashnode* next;
    void* key;
    void* data;
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

static void lazy_init(struct hashtable* ht)
{
    if (!ht->num_buckets)
        ht->num_buckets = 37;

    if (!ht->hashfunction)
        ht->hashfunction = standard_pointer_hash_function;

    if (!ht->cmpfunction)
        ht->cmpfunction = standard_pointer_comparison_function;

    if (!ht->buckets)
        ht->buckets = calloc(ht->num_buckets, sizeof(struct hashnode*));
}

void hashtable_reset(struct hashtable* ht)
{
    while (ht->size)
        hashtable_pop(ht);

    free(ht->buckets);
    ht->buckets = NULL;
}

static struct hashnode** findnodep(struct hashtable* ht, void* key)
{
    uint32_t hash;
    struct hashnode** hnp;

    lazy_init(ht);
    hash = ht->hashfunction(key) % ht->num_buckets;
    hnp = &ht->buckets[hash];

    while (*hnp && !ht->cmpfunction((*hnp)->key, key))
        hnp = &(*hnp)->next;

    return hnp;
}

void* hashtable_put(struct hashtable* ht, void* key, void* data)
{
    void* olddata;
    struct hashnode** hnp = findnodep(ht, key);
    if (!*hnp)
    {
        *hnp = calloc(1, sizeof(struct hashnode));
        ht->size++;
    }

    olddata = (*hnp)->data;
    (*hnp)->key = key;
    (*hnp)->data = data;
    return olddata;
}

void* hashtable_get(struct hashtable* ht, void* key)
{
    struct hashnode** hnp = findnodep(ht, key);
    if (*hnp)
        return (*hnp)->data;
    return NULL;
}

bool hashtable_contains(struct hashtable* ht, void* key)
{
    return *findnodep(ht, key);
}

bool hashtable_remove(struct hashtable* ht, void* key)
{
    struct hashnode** hnp = findnodep(ht, key);
    if (*hnp)
    {
        struct hashnode* hn = *hnp;
        *hnp = hn->next;
        free(hn);
        ht->size--;
        return true;
    }

    return false;
}

void* hashtable_pop(struct hashtable* ht)
{
    int i;

    if (ht->size == 0)
        return NULL;
    
    lazy_init(ht);
    for (i=0; i<ht->num_buckets; i++)
    {
        struct hashnode** hnp = &ht->buckets[i];
        if (*hnp)
        {
            struct hashnode* hn = *hnp;
            void* data = hn->data;
            *hnp = hn->next;
            free(hn);
            ht->size--;
            return data;
        }
    }

    return NULL;
}

void* hashtable_next(struct hashtable* ht, struct hashtable_iterator* it)
{
    while (!it->node)
    {
        if (it->bucket == ht->num_buckets)
        {
            it->data = NULL;
            it->bucket = 0;
            it->node = NULL;
            return NULL;
        }

        it->node = ht->buckets[it->bucket];
        if (it->node)
            break;

        it->bucket++;
    }

    it->data = it->node->data;
    it->node = it->node->next;
    if (!it->node)
        it->bucket++;

    return it->data;
}
