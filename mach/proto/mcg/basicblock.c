#include "mcg.h"

static void init_idf();
static struct idf* str2idf(char* tg, int cp);

#define IDF_TYPE struct basicblock*
#define IDF_NAME block
#include <idf_pkg.spec>
#include <idf_pkg.body>

static int next_id = 0;

void bb_init(void)
{
	init_idf();
}

struct basicblock* bb_get(const char* name)
{
	struct idf* p;
	
	if (!name)
		name = aprintf("___anon_bb_%d", next_id++);
	p = str2idf((char*) name, 0);
	if (!p->block)
	{
		p->block = calloc(sizeof(struct basicblock), 1);
		p->block->name = name;
	}
	return p->block;
}

void bb_alias(struct basicblock* block, const char* name)
{
	struct idf* p = str2idf((char*) name, -1);
	assert(p == NULL);

	p = str2idf((char*) name, 0);
	p->block = block;
}

/* vim: set sw=4 ts=4 expandtab : */
