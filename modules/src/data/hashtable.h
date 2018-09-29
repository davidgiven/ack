#ifndef HASHTABLE_H
#define HASHTABLE_H

/* A simple, autoresizing hash table. */

typedef uint32_t hashfunction_t(void* key);
typedef bool cmpfunction_t(void* key1, void* key2);
typedef void* allocfunction_t(size_t nmemb, size_t size);
typedef void freefunction_t(void* ptr);

extern uint32_t standard_int_hash_function(void* key);
extern bool standard_int_comparison_function(void* key1, void* key2);

extern uint32_t standard_pointer_hash_function(void* key);
extern bool standard_pointer_comparison_function(void* key1, void* key2);

extern uint32_t standard_string_hash_function(void* key);
extern bool standard_string_comparison_function(void* key1, void* key2);

struct hashtable
{
    hashfunction_t* hashfunction;
    cmpfunction_t* cmpfunction;
    allocfunction_t* allocfunction;
    freefunction_t* freefunction;
    unsigned int num_buckets;  /* power of 2 */
    struct hashnode** buckets;
    int size;
};

#define HASHTABLE_OF_INTS \
	{ standard_int_hash_function, standard_int_comparison_function }

#define HASHTABLE_OF_STRINGS \
	{ standard_string_hash_function, standard_string_comparison_function }

struct hashtable_iterator
{
    /* Public */
    void* key;
	void* value;

    /* Private */
    bool running;
    bool advanced;
    int bucket;
    struct hashnode* node;
};

extern void hashtable_empty(struct hashtable* ht);
extern void hashtable_reset(struct hashtable* ht);
extern void hashtable_rebucket(struct hashtable* ht, unsigned int num_buckets);

extern void* hashtable_put(struct hashtable* ht, void* key, void* data);
extern void* hashtable_puti(struct hashtable* ht, int key, void* data);
extern void* hashtable_get(struct hashtable* ht, void* key);
extern void* hashtable_geti(struct hashtable* ht, int key);
extern void* hashtable_remove(struct hashtable* ht, void* key);
extern void* hashtable_pop(struct hashtable* ht);
extern void* hashtable_next(struct hashtable* ht, struct hashtable_iterator* it);
extern void hashtable_delete_current(struct hashtable* ht, struct hashtable_iterator* it);

extern void hashtable_copy_all(struct hashtable* src, struct hashtable* dest);

#endif
