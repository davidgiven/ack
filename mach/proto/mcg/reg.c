#include "mcg.h"

int vreg_count = 1;
static struct buffer renderbuf;
static struct mempool regpool;

void clear_registers(void)
{
	tracef('M', "M: reg mempool was %d bytes\n", regpool.size);
	mempool_reset(&regpool);
	vreg_count = 1;
}

struct vreg* new_vreg(void)
{
	struct vreg* vreg = mempool_alloc(&regpool, sizeof *vreg);
	vreg->id = vreg_count++;
	vreg->spillslot = -1;
	return vreg;
}

struct hreg* new_hreg(const struct burm_register_data* brd)
{
	struct hreg* hreg = mempool_alloc(&regpool, sizeof *hreg);
	hreg->id = brd->id;
	hreg->brd = brd;
	hreg->regclasses = brd->attrs;
	/* The aliases array needs to be initialised later. */
	return hreg;
}

const char* render_regclass(regclass_t regclass)
{
	return burm_register_class_data[regclass].name;
}

const char* render_regclasses(uint32_t attrs)
{
	int i = 0;

	buffer_clear(&renderbuf);
	while (attrs)
	{
		if (attrs & 1)
		{
			if (renderbuf.len != 0)
				buffer_appendf(&renderbuf, "+");
			buffer_appendf(&renderbuf, "%s", render_regclass(i));
		}

		i++;
		attrs >>= 1;
	}

	return renderbuf.ptr;
}

const char* render_vreg(struct vreg* vreg)
{
	buffer_clear(&renderbuf);
	buffer_appendf(&renderbuf, "%%%d", vreg->id);
	if (vreg->in_transit)
		buffer_appendf(&renderbuf, "T");

	if (vreg->coalesced_with)
	{
		vreg = vreg->next_coalesced_register;
		while (vreg)
		{
			buffer_appendf(&renderbuf, "+%%%d", vreg->id);
			vreg = vreg->next_coalesced_register;
		}
	}

	return renderbuf.ptr;
}

/* Given a register class, finds the most generic compatible class for it. */

regclass_t find_generic_class_for(regclass_t regclass)
{
	int sizebits = burm_register_class_data[regclass].sizebits;
	switch (sizebits)
	{
		case burm_int_ATTR:
			return burm_int_CLASS;
		case burm_float_ATTR:
			return burm_float_CLASS;
		case burm_long_ATTR:
			return burm_long_CLASS;
		case burm_double_ATTR:
			return burm_double_CLASS;
	}
	fatal("register class %x has an ambiguous or unset size");
}

struct vreg* root_vreg_of(struct vreg* vreg)
{
	if (vreg->coalesced_with)
		return vreg->coalesced_with;
	return vreg;
}


/* vim: set sw=4 ts=4 expandtab : */
