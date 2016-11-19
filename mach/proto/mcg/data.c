#include "mcg.h"
#include <ctype.h>

#define IEEEFLOAT
#define FL_MSL_AT_LOW_ADDRESS 1
#define FL_MSW_AT_LOW_ADDRESS 1
#define FL_MSB_AT_LOW_ADDRESS 1

#include "con_float"

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

		fprintf(outputfile, "\n.sect %s\n", section_to_str(pending->section));
        fprintf(outputfile, "%s:\n", platform_label(pending->name));
        pending = NULL;
    }
}

static void writehex(arith data, int size)
{
    if (data < 0)
        fprintf(outputfile, "-0x%0*lx", size*2, -data); 
    else
        fprintf(outputfile, "0x%0*lx", size*2, data);
}

void data_int(arith data, size_t size, bool is_ro)
{
	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);
    assert((size == 1) || (size == 2) || (size == 4) || (size == 8));
    fprintf(outputfile, "\t.data%d ", size);
    writehex(data, size);
    fprintf(outputfile, "\n");
}


void data_float(const char* data, size_t size, bool is_ro)
{
    unsigned char buffer[8];
    int i;

	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);
    assert((size == 4) || (size == 8));

    i = float_cst(data, size, (char*) buffer);
    if ((i != 0) && (i != 2)) /* 2 == overflow */
        fatal("cannot parse floating point constant %s sz %d", data, size);

    fprintf(outputfile, "\t!float %s sz %d\n", data, size);
    fprintf(outputfile, "\t.data1 ");
    writehex(buffer[0], 1);
    for (i=1; i<size; i++)
    {
        fprintf(outputfile, ", ");
        writehex(buffer[i], 1);
    }
    fprintf(outputfile, "\n");
}

static bool istext(c)
{
    return isprint(c) && (c != '"');
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
        while ((p < end) && istext(*p))
            p++;

        if (start < p)
        {
            fprintf(outputfile, "\t.ascii \"");
            while (start < p)
            {
                fprintf(outputfile, "%c", *start);
                start++;
            }
            fprintf(outputfile, "\"\n");
        }

        while ((p < end) && !istext(*p))
            p++;

        if (start < p)
        {
            bool first = true;

            fprintf(outputfile, "\t.data1 ");
            while (start < p)
            {
                if (!first)
                    fprintf(outputfile, ", ");
                writehex(*start, 1);
                start++;
                first = false;
            }
            fprintf(outputfile, "\n");
        }
    }
}

void data_offset(const char* label, arith offset, bool is_ro)
{
	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);
    fprintf(outputfile, "\t.data%d %s+%lld\n",
        EM_pointersize, platform_label(label), offset);
}

void data_bss(arith size, int init)
{
    if (init != 0)
        fatal("non-zero-initialised bss not supported");

    emit_header(SECTION_BSS);
    fprintf(outputfile, "\t.space %lld\n", size);
}

/* vim: set sw=4 ts=4 expandtab : */
