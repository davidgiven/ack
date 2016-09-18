#include "mcg.h"
#include <ctype.h>

static struct symbol* pending;

void data_label(const char* label)
{
	if (pending)
		fatal("two consecutive data labels ('%s' and '%s')",
            pending->name, label);

	pending = symbol_get(label);
    if (pending->is_defined)
        fatal("label '%s' defined twice", pending->name);
    pending->is_defined = true;
}

static const char* section_to_str(int section)
{
	switch (section)
	{
		case SECTION_ROM:		return ".rom";
		case SECTION_DATA:		return ".data";
		case SECTION_BSS:		return ".bss";
		case SECTION_TEXT:		return ".text";
		default:             	return "unknown";
	}
}

static void emit_header(int desired_section)
{
	if (pending)
	{
		if (pending->section == SECTION_UNKNOWN)
			pending->section = desired_section;
		else if (pending->section != desired_section)
			fatal("label '%s' can't change sections", pending->name);

		printf("\n.sect %s\n", section_to_str(pending->section));
        printf("%s:\n", pending->name);
        pending = NULL;
    }
}

void data_int(arith data, size_t size, bool is_ro)
{
	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);
    assert((size == 1) || (size == 2) || (size == 4) || (size == 8));
    printf("\t.data%d 0x%0*lld\n", size, size*2, data);
}

void data_block(const uint8_t* data, size_t size, bool is_ro)
{
    const uint8_t* start = data;
    const uint8_t* end = data + size;
    const uint8_t* p = data;

	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);

    start = p = data;
    while (p < end)
    {
        while ((p < end) && isprint(*p))
            p++;

        if (start < p)
        {
            printf("\t.ascii \"");
            while (start < p)
            {
                printf("%c", *start);
                start++;
            }
            printf("\"\n");
        }

        while ((p < end) && !isprint(*p))
            p++;

        if (start < p)
        {
            bool first = true;

            printf("\t.data1 ");
            while (start < p)
            {
                if (!first)
                    printf(", ");
                printf("0x%02x", *start);
                start++;
                first = false;
            }
            printf("\n");
        }
    }
}

void data_offset(const char* label, arith offset, bool is_ro)
{
	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);
    printf("\t.data%d %s+%lld\n", EM_pointersize, label, offset);
}

void data_bss(arith size, int init)
{
    if (init != 0)
        fatal("non-zero-initialised bss not supported");

    emit_header(SECTION_BSS);
    printf("\t.space %lld\n", size);
}

/* vim: set sw=4 ts=4 expandtab : */
