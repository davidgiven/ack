#include "mcg.h"

static int vreg_count = 1;
static struct buffer renderbuf;

struct vreg* new_vreg(void)
{
	struct vreg* vreg = calloc(1, sizeof *vreg);
	vreg->id = vreg_count++;
	return vreg;
}

struct hreg* new_hreg(const struct burm_register_data* brd)
{
	struct hreg* hreg = calloc(1, sizeof *hreg);
	hreg->id = brd->id;
    hreg->brd = brd;
	hreg->attrs = brd->attrs;
    /* The aliases array needs to be initialised later. */
	return hreg;
}

const char* render_regattrs(uint32_t attrs)
{
    int i = 0;

    buffer_clear(&renderbuf);
    while (attrs)
    {
        if (attrs & 1)
        {
            if (renderbuf.len != 0)
                buffer_appendf(&renderbuf, "+");
            buffer_appendf(&renderbuf, "%s", burm_register_class_names[i]);
        }

        i++;
        attrs >>= 1;
    }

    return renderbuf.ptr;
}

/* vim: set sw=4 ts=4 expandtab : */

