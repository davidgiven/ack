#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "mcgg.h"
#include "registers.h"
#include "smap.h"
#include "array.h"
#include "stringlist.h"
#include "iburg.h"
#include "registers.h"

static SMAPOF(struct reg) registers;
static SMAPOF(struct regattr) registerattrs;

struct reg* makereg(const char* id)
{
	struct reg* p = smap_get(&registers, id);
	static int number = 0;

	if (p)
		yyerror("redefinition of '%s'", id);
	p = calloc(1, sizeof(*p));
	p->name = id;
	p->number = number++;
	array_append(&p->aliases, p);
	smap_put(&registers, id, p);

	return p;
}

void setregnames(struct reg* reg, struct stringlist* names)
{
	if (reg->names)
		yyerror("you can only set one set of register names");

	reg->names = names;
}

struct regattr* findregattr(const char* id)
{
	return smap_get(&registerattrs, id);
}

struct regattr* makeregattr(const char* id)
{
	struct regattr* p = smap_get(&registerattrs, id);
	static int number = 0;

	if (p)
		yyerror("redefinition of '%s'", id);
	p = calloc(1, sizeof(*p));
	p->name = id;
	p->number = number++;
	smap_put(&registerattrs, id, p);

	return p;
}

void addregattr(struct reg* reg, const char* id)
{
	struct regattr* p = smap_get(&registerattrs, id);

	if (!p)
		p = makeregattr(id);

	reg->attrs |= 1<<(p->number);
}

void addregalias(struct reg* r1, struct reg* r2)
{
	if (!array_appendu(&r1->aliases, r2))
	{
		int i;

		for (i=0; i<r1->aliases.count; i++)
			addregalias(r1->aliases.item[i], r2);
	}
}

void addregaliases(struct reg* reg, struct stringlist* aliases)
{
	struct stringfragment* f = aliases->first;

	while (f)
	{
		struct reg* r = smap_get(&registers, f->data);
		if (!r)
			yyerror("register '%s' is not defined here", f->data);

		array_appendu(&reg->aliases, r);
		array_appendu(&r->aliases, reg);

		f = f->next;
	}
}

struct regattr* getregattr(const char* id)
{
	struct regattr* p = smap_get(&registerattrs, id);
	if (!p)
		yyerror("'%s' is not the name of a register class", id);
	return p;
}

void emitregisterattrs(void)
{
	int i;

	print("const char* %Pregister_class_names[] = {\n");
	for (i=0; i<registerattrs.count; i++)
	{
		struct regattr* rc = registerattrs.item[i].right;
		assert(rc->number == i);

		print("%1\"%s\",\n", rc->name);
		printh("#define %P%s_ATTR (1U<<%d)\n", rc->name, rc->number);
	}
	print("};\n\n");
	printh("\n");
}

void emitregisters(void)
{
	int i, j;

	for (i=0; i<registers.count; i++)
	{
		struct reg* r = registers.item[i].right;

		print("const struct %Pregister_data* %Pregister_aliases_%d_%s[] = {\n%1", i, r->name);
		for (j=0; j<r->aliases.count; j++)
			print("&%Pregister_data[%d], ", r->aliases.item[j]->number);
		print("NULL\n};\n");
	}

	for (i=0; i<registers.count; i++)
	{
		struct reg* r = registers.item[i].right;

		print("const char* %Pregister_names_%d_%s[] = {\n%1", i, r->name);
		if (r->names)
		{
			struct stringfragment* f = r->names->first;
			while (f)
			{
				print("\"%s\", ", f->data);
				f = f->next;
			}
		}
		else
			print("\"%s\", ", r->name);
		print("NULL\n};\n");
	}

	print("const struct %Pregister_data %Pregister_data[] = {\n");
	for (i=0; i<registers.count; i++)
	{
		struct reg* r = registers.item[i].right;
		assert(r->number == i);

		print("%1{ \"%s\", 0x%x, %Pregister_names_%d_%s, %Pregister_aliases_%d_%s },\n",
			r->name, r->attrs, i, r->name, i, r->name);
	}
	print("%1{ NULL }\n");
	print("};\n\n");
}


