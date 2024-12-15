/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include "position.h"
#include "langdep.h"

struct langlist
{
	struct langlist* l_next;
	struct langdep* l_lang;
	char* l_suff;
};

/* STATICALLOCDEF "langlist" 5 */

static struct langlist* list;

struct langdep* currlang;

static void add_language(char* suff, struct langdep* lang)
{
	struct langlist* p = new_langlist();

	p->l_next = list;
	p->l_suff = suff;
	p->l_lang = lang;
	list = p;
}

void init_languages(void)
{
	add_language(".p", pascal_dep);
	add_language(".mod", m2_dep);
	add_language(".c", c_dep);
}

void find_language(char* suff)
{
	struct langlist* p = list;

	if (!suff)
	{
		currlang = c_dep;
		return;
	}
	while (p)
	{
		currlang = p->l_lang;
		if (!strcmp(p->l_suff, suff))
			break;
		p = p->l_next;
	}
	if (!currlang)
	{
		currlang = c_dep;
	}
}
