#include "mcg.h"
#include <ctype.h>

static struct symbol* pending;

void data_label(const char* label)
{
    struct symbol* sym = symbol_get(label);
    if (sym->is_defined)
        fatal("label '%s' defined twice", sym->name);

	if (pending)
        fprintf(outputfile, "%s = %s\n",
            platform_label(label), platform_label(pending->name));
    else
    {
        pending = sym;
        pending = symbol_get(label);
        pending->is_defined = true;
    }
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
    fprintf(outputfile, "\t.data%ld ", size);
    writehex(data, size);
    fprintf(outputfile, "\n");
}


void data_float(const char* data, size_t size, bool is_ro)
{
    unsigned char buffer[8];
    int i;

	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);
    assert((size == 4) || (size == 8));

    fprintf(outputfile, "\t.dataf%ld %s\n", size, data);
}

static bool istext(char c)
{
    return isprint(c) && (c != '"');
}

void data_block(const uint8_t* data, size_t datalen, size_t size, bool is_ro)
{
    const uint8_t* start = data;
    const uint8_t* end = data + datalen;
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
                if ((*start == '\\') || (*start == '"'))
                    fputc('\\', outputfile);
                fputc(*start, outputfile);
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

    for (size_t zeroes = 0; zeroes < (size - datalen); zeroes++)
        fprintf(outputfile, "\t.data1 0\n");
}

void data_offset(const char* label, arith offset, bool is_ro)
{
	emit_header(is_ro ? SECTION_ROM : SECTION_DATA);
    fprintf(outputfile, "\t.data%d %s+%ld\n",
        EM_pointersize, platform_label(label), offset);
}

void data_bss(arith size, int init)
{
    if (init != 0)
        fatal("non-zero-initialised bss not supported");

    emit_header(SECTION_BSS);
    fprintf(outputfile, "\t.space %ld\n", size);
}

/* vim: set sw=4 ts=4 expandtab : */
