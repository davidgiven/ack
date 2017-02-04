#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "registers.h"
#include "array.h"
#include "stringlist.h"
#include "iburg.h"
#include "registers.h"
#include "hashtable.h"
#include "bitmap.h"

static struct hashtable registers = HASHTABLE_OF_STRINGS;
static struct hashtable registerattrs = HASHTABLE_OF_STRINGS;

static struct reg** real_registers;
static int real_register_count;

static struct reg** fake_registers;
static int fake_register_count;

struct reg* makereg(const char* id)
{
	struct reg* p = hashtable_get(&registers, (void*)id);
	static int number = 0;

	if (p)
		yyerror("redefinition of '%s'", id);
	p = calloc(1, sizeof(*p));
	p->name = id;
	p->number = number++;
	hashtable_put(&registers, (void*)id, p);

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
	return hashtable_get(&registerattrs, (void*)id);
}

struct regattr* makeregattr(const char* id)
{
	struct regattr* p = hashtable_get(&registerattrs, (void*)id);
	static int number = 0;

	if (p)
		yyerror("redefinition of '%s'", id);
	p = calloc(1, sizeof(*p));
	p->name = id;
	p->number = number++;
	hashtable_put(&registerattrs, (void*)id, p);

	return p;
}

void addregattr(struct reg* reg, const char* id)
{
	struct regattr* p = hashtable_get(&registerattrs, (void*) id);

	if (!p)
		p = makeregattr(id);

	reg->attrs |= 1<<(p->number);
	set_add(&reg->classes, p);
}

void addregusage(struct reg* reg, struct stringlist* usage)
{
	struct stringfragment* f = usage->first;

	while (f)
	{
		struct reg* r = hashtable_get(&registers, (void*)f->data);
		if (!r)
			yyerror("register '%s' is not defined here", f->data);
		if (r->uses.count > 0)
			yyerror("'%s' can't use '%s' because the latter isn't a true hardware register",
				reg->name, r->name);

		array_appendu(&reg->uses, r);

		f = f->next;
	}
}

struct regattr* getregattr(const char* id)
{
	struct regattr* p = hashtable_get(&registerattrs, (void*)id);
	if (!p)
		yyerror("'%s' is not the name of a register class", id);
	return p;
}

void analyse_registers(void)
{
	struct reg* regs[registers.size];
	struct hashtable_iterator hit = {};
	int i, j;

	/* Collect registers in declaration order. */

	while (hashtable_next(&registers, &hit))
	{
		struct reg* r = hit.value;
		assert((r->number >= 0) && (r->number < registers.size));
		regs[r->number] = r;
	}

	real_registers = calloc(registers.size, sizeof(struct reg*));
	fake_registers = calloc(registers.size, sizeof(struct reg*));
	real_register_count = 0;
	fake_register_count = 0;

	/* Count which registers are real vs fake. */

	for (i=0; i<registers.size; i++)
	{
		struct reg* r = regs[i];
		if (r->uses.count > 0)
			fake_registers[fake_register_count++] = r;
		else
			real_registers[real_register_count++] = r;
	}

	/* Set the register bitmaps and weight. */

	for (i=0; i<real_register_count; i++)
	{
		struct reg* r = real_registers[i];
		r->bitmap = bitmap_alloc(real_register_count);
		bitmap_set(r->bitmap, real_register_count, i);
		r->weight = 1;
	}

	for (i=0; i<fake_register_count; i++)
	{
		struct reg* r = fake_registers[i];
		r->bitmap = bitmap_alloc(real_register_count);
		r->weight = r->uses.count;

		for (j=0; j<r->uses.count; j++)
		{
			struct reg* uses = r->uses.item[j];
			bitmap_or(r->bitmap, real_register_count, uses->bitmap);
		}
	}

	/* Set the register class bitmaps. */

	{
		struct hashtable_iterator hit = {};
		while (hashtable_next(&registerattrs, &hit))
		{
			struct regattr* rc = hit.value;
			rc->bitmap = bitmap_alloc(real_register_count);
		}
	}

	{
		struct hashtable_iterator hit = {};
		while (hashtable_next(&registers, &hit))
		{
			struct reg* r = hit.value;
			struct set_iterator sit = {};
			while (set_next(&r->classes, &sit))
			{
				struct regattr* rc = sit.item;
				bitmap_or(rc->bitmap, real_register_count, r->bitmap);

				if (r->weight > rc->weight)
					rc->weight = r->weight;
			}
		}
	}

	printh("#define %Pregister_count %d\n", real_register_count);
	printh("typedef unsigned int %Pregister_bitmap_t[%d];\n",
		WORDS_FOR_BITMAP_SIZE(real_register_count));
	printh("\n");
}

void emitregisterattrs(void)
{
	int i, j;
	struct regattr* regattrs[registerattrs.size];
	struct reg* regs[registers.size];

	{
		struct hashtable_iterator hit = {};
		while (hashtable_next(&registers, &hit))
		{
			struct reg* r = hit.value;
			assert((r->number >= 0) && (r->number < registers.size));
			regs[r->number] = r;
		}
	}

	{
		struct hashtable_iterator hit = {};
		while (hashtable_next(&registerattrs, &hit))
		{
			struct regattr* rc = hit.value;
			assert((rc->number >= 0) && (rc->number < registerattrs.size));
			regattrs[rc->number] = rc;
		}
	}

	for (i=0; i<registerattrs.size; i++)
	{
		struct regattr* rc = regattrs[i];
		print("static const struct %Pregister_data* %Pregisters_in_class_%s[] = {\n",
			rc->name);

		for (j=0; j<registers.size; j++)
		{
			struct reg* r = regs[j];
			if (set_get(&r->classes, rc))
				print("&%Pregister_data[%d], ", r->number);
		}

		print("NULL };\n");
	}

	print("\nconst struct %Pregclass_data %Pregclass_data[] = {\n");
	for (i=0; i<registerattrs.size; i++)
	{
		struct regattr* rc = regattrs[i];
		assert(rc->number == i);

		print("%1{ \"%s\", %Pregisters_in_class_%s, { ", rc->name, rc->name);
		for (j=0; j<WORDS_FOR_BITMAP_SIZE(real_register_count); j++)
		{
			print("0x%x, ", rc->bitmap[j]);
		}
		print("}, ");
		print("%d,", rc->weight);
		print("},\n");
		printh("#define %P%s_ATTR (1U<<%d)\n", rc->name, rc->number);
	}
	print("};\n\n");
	printh("\n");
}

void emitregisters(void)
{
	int i, j;
	struct reg* regs[registers.size];

	{
		struct hashtable_iterator hit = {};
		while (hashtable_next(&registers, &hit))
		{
			struct reg* r = hit.value;
			assert((r->number >= 0) && (r->number < registers.size));
			regs[r->number] = r;
		}
	}

	for (i=0; i<registers.size; i++)
	{
		struct reg* r = regs[i];

		print("static const char* %Pregister_names_%s[] = { ", r->name);
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
		print("NULL };\n");
	}

	print("\nconst struct %Pregister_data %Pregister_data[] = {\n");
	for (i=0; i<registers.size; i++)
	{
		struct reg* r = regs[i];

		assert(r->number == i);
		print("%1{ \"%s\", 0x%x, %Pregister_names_%s, ",
			r->name, r->attrs, r->name, r->name);
		print("{ ");
		for (j=0; j<WORDS_FOR_BITMAP_SIZE(real_register_count); j++)
		{
			print("0x%x, ", r->bitmap[j]);
		}
		print("}},\n");
	}
	print("};\n\n");
}


