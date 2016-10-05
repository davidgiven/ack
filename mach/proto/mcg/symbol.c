#include "mcg.h"

static void init_idf();
static struct idf* str2idf(char* tg, int cp);

#define IDF_TYPE struct symbol
#define IDF_NAME symbol
#include <idf_pkg.spec>
#include <idf_pkg.body>

void symbol_init(void)
{
	init_idf();
}

bool symbol_exists(const char* name)
{
	return !!findidf((char*) name);
}

struct symbol* symbol_get(const char* name)
{
	struct idf* p = str2idf((char*) name, 0);
	p->symbol.name = p->id_text;
	return &p->symbol;
}

void symbol_declare(const char* name, bool is_exported, bool is_proc)
{
	struct symbol* s = symbol_get(name);
	s->is_exported = is_exported;

	if (is_proc)
	{
		if (s->section == SECTION_UNKNOWN)
			s->section = SECTION_TEXT;
		else if (s->section != SECTION_TEXT)
			fatal("section mismatch for '%s'", name);
	}
}

struct symbol* symbol_walk(symbol_walker_t* cb, void* user)
{
	int i;

	for (i=0; i<IDF_HASHSIZE; i++)
	{
		struct idf* idf = IDF_hashtable[i];
		while (idf)
		{	
			struct symbol* symbol = &idf->symbol;
			if (cb(symbol, user))
				return symbol;
			idf = idf->id_next;
		}
	}

	return NULL;
}

/* vim: set sw=4 ts=4 expandtab : */

