/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Header$";
#endif

#include <out.h>
#include <stdio.h>
#include "const.h"
#include "assert.h"
#include "memory.h"

extern struct outhead	outhead;
extern struct outsect	outsect[];
extern int		flagword;
extern bool		incore;

wr_fatal()
{
	fatal("write error");
}

static long		off_char;

/*
 * Open the output file according to the chosen strategy.
 * Write away the header and section table: they will not change anymore.
 */
begin_write()
{
	extern char *outputname;
	register struct outhead *hd = &outhead;

	assert(! incore);
	if (! wr_open(outputname)) {
		fatal("cannot write %s", outputname);
	}
	wr_ohead(hd);
	wr_sect(outsect, hd->oh_nsect);
	off_char = OFF_CHAR(*hd);
}

static struct outname *
sectname(sectindex)
	int			sectindex;
{
	static struct outname	namebuf;

	namebuf.on_foff = (long)0;	/* No string name. */
	namebuf.on_type = (S_MIN + sectindex) | S_SCT;
	namebuf.on_desc = 0;
	namebuf.on_valu = outsect[sectindex].os_base;

	return &namebuf;
}

/*
 * Write out the symbol table and the section names.
 */
end_write()
{
	register ushort		cnt;
	register struct outname	*name;
	register int		sectindex;
	extern ushort		NGlobals;

	assert(!incore);
	assert(!(flagword & SFLAG));
	cnt = NGlobals;
	name = (struct outname *)address(ALLOGLOB, (ind_t)0);
	while (cnt--) {
		if (name->on_foff != (long)0) {
			name->on_mptr = address(ALLOGCHR, (ind_t)name->on_foff);
		} else {
			name->on_mptr = (char *)0;
		}
		wrt_name(name);
		name++;
	}

	for (sectindex = 0; sectindex < outhead.oh_nsect; sectindex++)
		wrt_name(sectname(sectindex));
}
	
wrt_emit(emit, sectindex, cnt)
	char		*emit;
	int		sectindex;
	long		cnt;
{

	wr_outsect(sectindex);
	wr_emit(emit, cnt);
}

wrt_nulls(sectindex, cnt)
	register long cnt;
{
	static char nullbuf[BUFSIZ];

	wr_outsect(sectindex);
	while (cnt) {
		register int n = cnt >= BUFSIZ ? BUFSIZ : cnt;
		wr_emit(nullbuf, (long)n);
		cnt -= n;
	}
}

wrt_name(name)
	register struct outname	*name;
{
	assert(!incore);
	assert(!(flagword & SFLAG));
	if (name->on_mptr != (char *)0) {
		register long	len = strlen(name->on_mptr) + 1;

		wr_string(name->on_mptr, len);
		name->on_foff = off_char;
		off_char += len;
	} else {
		name->on_foff = (long)0;
	}
	wr_name(name, 1);
}
