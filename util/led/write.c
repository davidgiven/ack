#ifndef lint
static char rcsid[] = "$Header$";
#endif

/*
 * You can choose between two strategies:
 *	- Open the output file several times, once for each logical part, and
 *	write to it in multiple places.
 *	- Open the output file once and seek back and forth to each logical
 *	part. In this case #define OUTSEEK.
 */

#include <stdio.h>
#include "../../h/out.h"
#include "const.h"
#include "assert.h"
#include "memory.h"
#include "orig.h"

extern long		lseek();

#define WRITE		1	/* Argument to open(). */

/*
 * Parts of the output file.
 */
#define	PARTEMIT	0
#define	PARTRELO	1
#define	PARTNAME	2
#define	PARTCHAR	3
#ifdef SYMDBUG
#define PARTDBUG	4
#else SYMDBUG
#define PARTDBUG	PARTCHAR
#endif SYMDBUG
#define	NPARTS		(PARTDBUG + 1)

/*
 * Call OUTPART() with the part you want to write on as argument, before
 * you call OUTWRITE().
 */
static int		outpart = NPARTS;

#ifdef OUTSEEK

static int		outfile;
static long		outseek[NPARTS];

#define	OUTPART(p) \
	{	if (outpart != (p)) {\
			outpart = (p);\
			lseek(outfile, outseek[(p)], 0);\
		}\
	}
#define OUTSECT(i) \
	{	outpart = NPARTS;\
		outseek[PARTEMIT] =\
			outsect[(i)].os_foff + relorig[(i)].org_flen;\
	}
#define OUTWRITE(b, n) \
	{	if (write(outfile, (b), (n)) != (n))\
			fatal("write error");\
		outseek[outpart] += (n);\
	}
#define BEGINSEEK(p, o) \
	{	outseek[(p)] = (o);\
	}

#else OUTSEEK

static int	outfile[NPARTS];

#define	OUTPART(p) \
	{	outpart = (p);\
	}
#define OUTSECT(i) \
	{	lseek(	outfile[PARTEMIT],\
			outsect[(i)].os_foff + relorig[(i)].org_flen,\
			0\
		);\
	}
#define OUTWRITE(b, n) \
	{	if (write(outfile[outpart], (b), (n)) != (n))\
			fatal("write error");\
	}
#define	BEGINSEEK(p, o) \
	{	lseek(outfile[(p)], (o), 0);\
	}

#endif OUTSEEK

extern struct outhead	outhead;
extern struct outsect	outsect[];
extern int		flagword;
extern struct orig	relorig[];
extern bool		bytes_reversed, words_reversed;
extern bool		incore;


static long		offchar;

/*
 * Open the output file according to the chosen strategy.
 * Write away the header and section table: they will not change anymore.
 */
begin_write()
{
	register long	off;

	openoutput();
	BEGINSEEK(PARTEMIT, (long)0);
	wrt_head(&outhead);
	wrt_sect(outsect, outhead.oh_nsect);

	off = SZ_HEAD + (long)outhead.oh_nsect * SZ_SECT + outhead.oh_nemit;
	
	if (flagword & RFLAG) {
		/* A relocation table will be produced. */
		BEGINSEEK(PARTRELO, off);
		off += (long)outhead.oh_nrelo * SZ_RELO;
	}

	if (flagword & SFLAG)
		return;

	/* A name table will be produced. */
	BEGINSEEK(PARTNAME, off);
	off += (long)outhead.oh_nname * SZ_NAME;
	BEGINSEEK(PARTCHAR, off);
	offchar = off; /* See wrt_name(). */
#ifdef SYMDBUG
	off += outhead.oh_nchar;
	BEGINSEEK(PARTDBUG, off);
#endif SYMDBUG
}

static
openoutput()
{
#ifndef OUTSEEK
	register int	*fdp;
#endif OUTSEEK
	extern char	*outputname;

	close(creat(outputname, 0666));
#ifdef OUTSEEK
	if ((outfile = open(outputname, WRITE)) < 0)
		fatal("can't write %s", outputname);
#else OUTSEEK
	for (fdp = &outfile[PARTEMIT]; fdp < &outfile[NPARTS]; fdp++)
		if ((*fdp = open(outputname, WRITE)) < 0)
			fatal("can't write %s", outputname);
#endif OUTSEEK
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
	
static
wrt_head(head)
	register struct outhead	*head;
{
	assert(!incore);
	OUTPART(PARTEMIT);
	if (bytes_reversed || words_reversed)
		swap((char *)head, SF_HEAD);
	OUTWRITE((char *)head, SZ_HEAD);
	/*
	 * Swap back because we will need it again.
	 */
	if (bytes_reversed || words_reversed)
		swap((char *)head, SF_HEAD);
}

static
wrt_sect(sect, cnt)
	register struct outsect	*sect;
	register ushort		cnt;
{
	assert(!incore);
	OUTPART(PARTEMIT);
	while (cnt--) {
		if (bytes_reversed || words_reversed)
			swap((char *)sect, SF_SECT);
		OUTWRITE((char *)sect, SZ_SECT);
		/*
		 * Swap back because we will need it again.
		 */
		if (bytes_reversed || words_reversed)
			swap((char *)sect, SF_SECT);
		sect++;
	}
}

/*
 * We don't have to worry about byte order here.
 */
wrt_emit(emit, sectindex, cnt)
	register char		*emit;
	int			sectindex;
	register long		cnt;
{
	register int		n;

	assert(!incore);
	OUTPART(PARTEMIT);
	OUTSECT(sectindex);
	while (cnt) {
		n = cnt >= BUFSIZ ? BUFSIZ : cnt;
		OUTWRITE(emit, n);
		emit += n;
		cnt -= n;
	}
}

wrt_relo(relo)
	register struct outrelo	*relo;
{
	assert(!incore);
	assert(flagword & RFLAG);
	OUTPART(PARTRELO);
	if (bytes_reversed || words_reversed)
		swap((char *)relo, SF_RELO);
	OUTWRITE((char *)relo, SZ_RELO);
}		

wrt_name(name)
	register struct outname	*name;
{
	assert(!incore);
	assert(!(flagword & SFLAG));
	if (name->on_mptr != (char *)0) {
		register int	len = strlen(name->on_mptr) + 1;

		OUTPART(PARTCHAR);
		OUTWRITE(name->on_mptr, len);
		name->on_foff = offchar;
		offchar += len;
	} else {
		name->on_foff = (long)0;
	}
	OUTPART(PARTNAME);
	if (bytes_reversed || words_reversed)
		swap((char *)name, SF_NAME);
	OUTWRITE((char *)name, SZ_NAME);
}
#ifdef SYMDBUG

wrt_dbug(buf, size)
	char		*buf;
	int		size;
{
	assert(!incore);
	assert(!(flagword & SFLAG));
	OUTPART(PARTDBUG);
	OUTWRITE((char *)buf, size);
}
#endif SYMDBUG
