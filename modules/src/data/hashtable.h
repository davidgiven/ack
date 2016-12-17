#ifndef HASHTABLE_H
#define HASHTABLE_H

/* A simple, autoresizing hash table. */

typedef uint32_t hashfunction_t(void* key);
typedef bool cmpfunction_t(void* key1, void* key2);

extern uint32_t standard_pointer_hash_function(void* key);
extern bool standard_pointer_comparison_function(void* key1, void* key2);

struct hashtable
{
    unsigned int num_buckets;  /* power of 2 */
    hashfunction_t* hashfunction;
    cmpfunction_t* cmpfunction;
    struct hashnode** buckets;
    int size;
};

struct hashtable_iterator
{
    /* Public */
    void* key;
	void* value;

    /* Private */
    int bucket;
    struct hashnode* node;
};

extern void hashtable_empty(struct hashtable* ht);
extern void hashtable_reset(struct hashtable* ht);
extern void hashtable_rebucket(struct hashtable* ht, unsigned int num_buckets);

extern void* hashtable_put(struct hashtable* ht, void* key, void* data);
extern void* hashtable_get(struct hashtable* ht, void* key);
extern void* hashtable_remove(struct hashtable* ht, void* key);
extern void* hashtable_pop(struct hashtable* ht);
extern void* hashtable_next(struct hashtable* ht, struct hashtable_iterator* it);

#endif
