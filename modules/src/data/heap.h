#ifndef HEAP_H
#define HEAP_H

struct heap
{
    struct heap_node* next;
};

extern void* heap_alloc(struct heap* heap, size_t nmemb, size_t size);
extern void heap_free(struct heap* heap);

#endif
