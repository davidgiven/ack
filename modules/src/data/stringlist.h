#ifndef STRINGLIST_H
#define STRINGLIST_H

struct stringfragment
{
	const char* data;
	struct stringfragment* next;
};

struct stringlist
{
	struct stringfragment* first;
	struct stringfragment* last;
};

extern void stringlist_add(struct stringlist* list, const char* data);
extern void stringlist_addall(struct stringlist* list, struct stringlist* src);
extern void stringlist_free(struct stringlist* list);

#endif

/* vim: set sw=4 ts=4 expandtab : */

