/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * You can choose between two strategies:
 *	- Open the output file several times, once for each logical part, and
 *	write to it in multiple places.
 *	- Open the output file once and seek back and forth to each logical
 *	part. In this case #define OUTSEEK.
 */

#include <out.h>
#include "object.h"

extern long		lseek();

/*
 * Parts of the output file.
 */
#define	PARTEMIT	0
#define	PARTRELO	1
#define	PARTNAME	2
#define	PARTCHAR	3
#ifdef SYMDBUG
#define PARTDBUG	4
#else
#define PARTDBUG	3
#endif
#define	NPARTS		(PARTDBUG + 1)

static long		offset[MAXSECT];

#ifdef OUTSEEK
static int		outfile;
static long		outseek[NPARTS];
static long		currpos;
#define OUTSECT(i) \
	(outseek[PARTEMIT] = offset[i])
static
OUTWRITE(p, b, n) {
	char *b;
	long n;
{
	register long l = outseek[p];

	if (currpos != l) {
		lseek(outfile, l, 0);
	}
	wr_bytes(outfile, b, n);
	currpos = l + n;
	outseek[p] = currpos;
}

#define BEGINSEEK(p, o) \
	(outseek[(p)] = (o))

#else OUTSEEK

static int	outfile[NPARTS];
static long	currpos[NPARTS];
#define OUTSECT(i) \
	(currpos[PARTEMIT] == offset[(i)] ?\
		0 :\
		(currpos[PARTEMIT] = offset[(i)],\
		 lseek(outfile[PARTEMIT], currpos[PARTEMIT], 0)))
#define OUTWRITE(p, b, n) \
	(wr_bytes(outfile[(p)], (b), (n)), currpos[(p)] += (n))
#define BEGINSEEK(p, o) \
	(currpos[(p)] = lseek(outfile[(p)], (o), 0))

#endif OUTSEEK

int _ocnt;
char *_pbuf;
static int sectionnr;
static int offcnt;

/*
 * Open the output file according to the chosen strategy.
 */
int
wr_open(f)
	char *f;
{
#ifndef OUTSEEK
	register int	*fdp;
#endif OUTSEEK

	close(creat(f, 0666));
#ifdef OUTSEEK
	if ((outfile = open(f, 1)) < 0)
		return 0;
	currpos = 0;
#else OUTSEEK
	for (fdp = &outfile[PARTEMIT]; fdp < &outfile[NPARTS]; fdp++)
		if ((*fdp = open(f, 1)) < 0)
			return 0;
#endif OUTSEEK
	offcnt = 0;
	return 1;
}

wr_close()
{
#ifndef OUTSEEK
	register int *fdp;
#endif not OUTSEEK

	if (_ocnt) {
		wr_flush();
	}
#ifdef OUTSEEK
	close(outfile);
	outfile = -1;
#else not OUTSEEK
	for (fdp = &outfile[PARTEMIT]; fdp < &outfile[NPARTS]; fdp++) {
		close(*fdp);
		*fdp = -1;
	}
#endif not OUTSEEK
}

wr_fd()
{
#ifdef OUTSEEK
	return outfile;
#else
	return outfile[PARTEMIT];
#endif
}

wr_ohead(head)
	register struct outhead	*head;
{
	register long off = OFF_RELO(*head);

	BEGINSEEK(PARTEMIT, 0L);
	BEGINSEEK(PARTRELO, off);
	off += (long) head->oh_nrelo * SZ_RELO;
	BEGINSEEK(PARTNAME, off);
	off += (long) head->oh_nname * SZ_NAME;
	BEGINSEEK(PARTCHAR, off);
#ifdef SYMDBUG
	off += head->oh_nchar;
	BEGINSEEK(PARTDBUG, off);
#endif
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	if (sizeof(struct outhead) != SZ_HEAD)
#endif
	{
		char buf[SZ_HEAD];
		register char *c = buf;

		put2(head->oh_magic, c);	c += 2;
		put2(head->oh_stamp, c);	c += 2;
		put2(head->oh_flags, c);	c += 2;
		put2(head->oh_nsect, c);	c += 2;
		put2(head->oh_nrelo, c);	c += 2;
		put2(head->oh_nname, c);	c += 2;
		put4(head->oh_nemit, c);	c += 4;
		put4(head->oh_nchar, c);
		OUTWRITE(PARTEMIT, buf, (long) SZ_HEAD);
	}
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	else	OUTWRITE(PARTEMIT, (char *) head, (long) SZ_HEAD);
#endif
}

wr_sect(sect, cnt1)
	register struct outsect	*sect;
	unsigned int	cnt1;
{
	char buf[MAXSECT * SZ_SECT];
	register char *c = buf;
	register unsigned int cnt = cnt1;

	while (cnt--) {
#if ! (BYTES_REVERSED || WORDS_REVERSED)
		if (sizeof(struct outsect) != SZ_SECT)
#endif
		{
			put4(sect->os_base, c); c += 4;
			put4(sect->os_size, c); c += 4;
			put4(sect->os_foff, c); c += 4;
		}
		offset[offcnt++] = sect->os_foff;
#if ! (BYTES_REVERSED || WORDS_REVERSED)
		if (sizeof(struct outsect) != SZ_SECT)
#endif
		{
			put4(sect->os_flen, c); c += 4;
			put4(sect->os_lign, c); c += 4;
		}
		sect++;
	}
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	if (sizeof(struct outsect) != SZ_SECT)
#endif
		OUTWRITE(PARTEMIT, buf, (long) cnt1 * SZ_SECT);
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	else
		OUTWRITE(PARTEMIT, (char *) (sect - cnt1), (long) cnt1 * SZ_SECT);
#endif
}

wr_flush()
{
	OUTWRITE(PARTEMIT, _pbuf, (long) _ocnt);
	offset[sectionnr] += _ocnt;
	_ocnt = 0;
}

wr_outsect(s)
{
	if (s != sectionnr) {
		if (_ocnt) {
			wr_flush();
		}
		sectionnr = s;
		OUTSECT(s);
	}
}

/*
 * We don't have to worry about byte order here.
 */
wr_emit(emit, cnt)
	char		*emit;
	long		cnt;
{
	if (_ocnt) wr_flush();
	OUTWRITE(PARTEMIT, emit, cnt);
	offset[sectionnr] += cnt;
}

wr_relo(relo, cnt)
	register struct outrelo	*relo;
	register unsigned int cnt;
{
	long l;

#if ! (BYTES_REVERSED || WORDS_REVERSED)
	if (sizeof(struct outrelo) != SZ_RELO)
#endif
	{
		char buf[100 * SZ_RELO];
		register char *c = buf;
		register int i = 0;

		while (cnt--) {
			*c++ = relo->or_type;
			*c++ = relo->or_sect;
			put2(relo->or_nami, c); c += 2;
			put4(relo->or_addr, c); c += 4;
			relo++;
			i++;
			if (i == 100 || cnt == 0) {
				c = buf;
				l = (long) (i * SZ_RELO);
				OUTWRITE(PARTRELO, c, l);
				i = 0;
			}
		}
	}
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	else {
		l = (long) cnt * SZ_RELO;
		OUTWRITE(PARTRELO, (char *) relo, l);
	}
#endif
}

wr_name(name, cnt)
	register struct outname	*name;
	register unsigned int cnt;
{
	long l;

#if ! (BYTES_REVERSED || WORDS_REVERSED)
	if (sizeof(struct outname) != SZ_NAME)
#endif
	{
		char buf[100 * SZ_NAME];
		register char *c = buf;
		register int i = 0;

		while (cnt--) {
			put4(name->on_foff,c); c += 4;
			put2(name->on_type,c); c += 2;
			put2(name->on_desc,c); c += 2;
			put4(name->on_valu,c); c += 4;
			name++;
			i++;
			if (i == 100 || !cnt) {
				c = buf;
				l = (long) (i * SZ_NAME);
				OUTWRITE(PARTNAME, c, l);
				i = 0;
			}
		}
	}
#if ! (BYTES_REVERSED || WORDS_REVERSED)
	else {
		l = (long)cnt * SZ_NAME;
		OUTWRITE(PARTNAME, (char *) name, l);
	}
#endif

}

wr_string(addr, len)
	char *addr;
	long len;
{
	
	OUTWRITE(PARTCHAR, addr, len);
}

#ifdef SYMDBUG

wr_dbug(buf, size)
	char		*buf;
	long		size;
{
	OUTWRITE(PARTDBUG, buf, size);
}

#endif SYMDBUG
