/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * machine dependent back end routines for the eZ80.
 */

#include <stdlib.h> /* atol */

void con_part(int sz, word w)
{

	while (part_size % sz)
		part_size++;
	if (part_size == 3)
		part_flush();
	if (sz == 1)
	{
		w &= 0xFF;
		if (part_size)
			w <<= 8;
		part_word |= w;
	}
	else if (sz == 2)
	{
		w &= 0xffff;
		if (part_size)
			w <<= 16;
		part_word |= w;
	}
	part_size += sz;
}

void con_mult(word sz)
{
	switch (sz)
	{
		case 3:
			fprintf(codefile, ".data3 %s\n", (int)sz, str);
			break;

		case 6:
		{
			long long value = atoll(str);
			fprintf(codefile, ".data3 0x%lx\n", value & 0xffffff);
			fprintf(codefile, ".data3 0x%lx\n", value >> 24);
			break;
		}

		default:
			fatal("bad icon/ucon size");
	}
}

#define CODE_GENERATOR
#define IEEEFLOAT
#define FL_MSL_AT_LOW_ADDRESS 0
#define FL_MSW_AT_LOW_ADDRESS 0
#define FL_MSB_AT_LOW_ADDRESS 0
#include <con_float>

void prolog(full nlocals)
{
	int16_t adjustment = -nlocals;

	fprintf(codefile, "push iy\n");
	fprintf(codefile, "ld iy, 0\n");
	fprintf(codefile, "add iy, sp\n");
	if (adjustment != 0)
	{
		fprintf(codefile, "ld hl, %d\n", adjustment);
		fprintf(codefile, "add hl, sp\n");
		fprintf(codefile, "ld sp, hl\n");
	}
}

void mes(type) word type;
{
	int argt;

	switch ((int)type)
	{
		case ms_ext:
			for (;;)
			{
				switch (argt = getarg(ptyp(sp_cend) | ptyp(sp_pnam) | sym_ptyp))
				{
					case sp_cend:
						return;

					default:
						strarg(argt);
						fprintf(codefile, ".define %s\n", argstr);
						break;
				}
			}

		default:
			while (getarg(any_ptyp) != sp_cend)
				;
			break;
	}
}

char* segname[] = { ".sect .text", ".sect .data", ".sect .rom", ".sect .bss" };
