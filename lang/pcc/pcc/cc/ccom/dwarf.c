/*      $Id: dwarf.c,v 1.2 2016/03/08 18:17:45 ragge Exp $     */

/*
 * Copyright (c) 2016 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Expect just ELF for now.
 */
#include "pass1.h"

#ifdef DWARF

#include <sys/param.h>
#include <unistd.h>

#include "dwarf.h"

/*
 * Basic DWARF definitions, from specification.
 */
#define	DVERSION	3	/* dwarf version */
#define	DLABEL		".LD%d"

#define	DW64	(SZPOINT(VOID)/SZCHAR == 8)

static int dwseg = -1;
enum { DINFO, DABBREV, DSTR };
static char *segn[] = { ".debug_info", ".debug_abbrev", ".debug_str" };

static int debug_info_sz, debug_info_sz_lbl;
static int debug_abbrev_beg_lbl;

static int dwcfl, dwbtext, dwetext;

static char *dwname;

static int
dwlab(void)
{
	static int dwlbl;
	return dwlbl++;
}

static void
dwarfseg(int seg)
{
	if (seg == dwseg)
		return;
	/* XXX target code? */
	printf("\t.section\t%s,\"\",@progbits\n", segn[seg]);
	lastloc = NOSEG;
	dwseg = seg;
}

static void
dwslab(int seg, int lbl)
{
	dwarfseg(seg);
	printf(PRTPREF DLABEL ":\n", lbl);
}

static void
p1b(int v)
{
	printf(PRTPREF "%s 0x%x\n", astypnames[CHAR], v);
}

static void
p2w(int v)
{
	printf(PRTPREF "%s 0x%x\n", astypnames[SHORT], v);
}

static int
leb128(int d)
{
	if (d > 127)
		cerror("FIXME: leb128 > 127");
	p1b(d);
	return 1;
}

static void
al128(int d)
{
	dwarfseg(DABBREV);
	leb128(d);
}

static void
apair(int d, int e)
{
	dwarfseg(DABBREV);
	leb128(d);
	leb128(e);
}

static void
strng(char *s)
{
	printf(PRTPREF "\t.ascii \"%s\\0\"\n", s); /* XXX common code? */
}

static void
istring(char *str)
{
	dwarfseg(DINFO);
	strng(str);
	debug_info_sz += (strlen(str) + 1);
}

static void
dwslabstr(int lbl, char *s)
{
	dwslab(DSTR, lbl);
	strng(s);
}

static void
ilbl(int l)
{
	dwarfseg(DINFO);
	printf(PRTPREF "%s " DLABEL "\n", astypnames[LONG], l);
	debug_info_sz += (DW64 * 4 + 4);
}

static void
il128(int d)
{
	dwarfseg(DINFO);
	debug_info_sz += leb128(d);
}

static void
p412l(int lbl)
{
	if (DW64)
		printf(PRTPREF "%s 0xffffff00\n", astypnames[INT]);
	printf(PRTPREF "%s " DLABEL "\n", astypnames[LONG], lbl);
}

static void
info1b(int v)
{
	dwarfseg(DINFO);
	p1b(v);
	debug_info_sz++;
}

static void
info2w(int v)
{
	dwarfseg(DINFO);
	p2w(v);
	debug_info_sz += 2;
}

static void
info412l(int lbl)
{
	dwarfseg(DINFO);
	p412l(lbl);
	debug_info_sz += (DW64 * 8 + 4);
}

void
dwarf_init(char *iname)
{
	char buf[MAXPATHLEN];
	/*
	 * Setup sections.
	 * We'll print out the basic stuff early so that not
	 * so much context must be kept around.
	 */
	dwname = iname;
	debug_info_sz_lbl = dwlab();
	debug_abbrev_beg_lbl = dwlab();

	/* begin with setting up the compilation unit header */
	info412l(debug_info_sz_lbl); debug_info_sz = 0;
	info2w(DVERSION);
	info412l(debug_abbrev_beg_lbl);
	info1b(SZPOINT(VOID)/SZCHAR);

	/* Define abbrev table */
	dwslab(DABBREV, debug_abbrev_beg_lbl);

	/* compile unit follows, both in abbrev and info table */
	/* Recommended stuff here as in the documentation */
	al128(1); /* always first entry */
	al128(DW_TAG_compile_unit);
	al128(DW_CHILDREN_yes);

	apair(DW_AT_name, DW_FORM_strp); /* file name; e.g. foo.c */
	apair(DW_AT_producer, DW_FORM_string); /* producer; PCC 1.2.3 */
	apair(DW_AT_comp_dir, DW_FORM_string); /* working directory */
	apair(DW_AT_language, DW_FORM_data1); /* C99 */
	apair(DW_AT_low_pc, DW_FORM_addr); /* start text */
	apair(DW_AT_high_pc, DW_FORM_addr); /* end text */
	apair(0, 0);

	/* Above in info table */
	il128(1); /* first entry */
	ilbl(dwcfl = dwlab());
	istring(PACKAGE_STRING);
	istring(getcwd(buf, MAXPATHLEN));
	il128(DW_LANG_C99);
	ilbl(dwbtext = dwlab());
	ilbl(dwetext = dwlab());

	locctr(PROG, NULL);
	printf(PRTPREF DLABEL ":\n", dwbtext);
}

void
dwarf_file(char *fn)
{
	/* if first file name, print out as initial and remember */
	if (dwcfl) {
		dwslabstr(dwcfl, fn);
		dwcfl = 0;
	}
	/* XXX add more here */
}

void
dwarf_end()
{
	locctr(PROG, NULL);
	printf(PRTPREF DLABEL ":\n", dwetext);
	if (dwcfl)
		dwslabstr(dwcfl, dwname ? dwname : "<unknown>");
	printf(PRTPREF "\t.set " DLABEL ",%d\n",
	    debug_info_sz_lbl, debug_info_sz);
}

#endif
