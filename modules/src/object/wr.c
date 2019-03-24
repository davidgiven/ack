/* $Id$ */
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

#include <stdlib.h>
#include <stdio.h>
#include "out.h"
#include "object.h"
#include "obj.h"
#include "ranlib.h"

/*
 * Parts of the output file.
 */
static long		offset[MAXSECT];
struct fil		__parts[NPARTS];

#ifdef OUTSEEK
static FILE*	outfile;
static long		currpos;
#endif
int			__sectionnr;
#define sectionnr	__sectionnr
static int		offcnt;

void
__wr_flush(struct fil *ptr)
{
#ifdef OUTSEEK
	/* seek to correct position even if we aren't going to write now */
	if (currpos != ptr->currpos) {
		currpos = fseek(ptr->fd, ptr->currpos, SEEK_SET);
	}
#endif
	if (ptr->pnow > ptr->pbegin) {
		wr_bytes(ptr->fd, ptr->pbegin, (long)(ptr->pnow - ptr->pbegin));
		ptr->currpos += ptr->pnow - ptr->pbegin;
#ifdef OUTSEEK
		currpos = ptr->currpos;
#endif
		if (ptr < &__parts[PARTEMIT+SECTCNT]) {
			offset[sectionnr] = ptr->currpos;
		}
	}
	ptr->cnt = WBUFSIZ;
	ptr->pnow = ptr->pbuf;
	ptr->pbegin = ptr->pbuf;
}

static void
OUTWRITE(int p, const char *b, long n)
	/* p = part number, b = buffer pointer, n = write count */
{
	struct fil *ptr = &__parts[p];
	char *pn = ptr->pnow;
	int i;
	long m;

	i = ptr->cnt;
	if (i < WBUFSIZ) {
		if (n > i) {
			__wr_flush(ptr);
			wr_bytes(ptr->fd, b, (long) i);
			n -= i;
			b += i;
			ptr->currpos += i;
#ifdef OUTSEEK
			currpos = ptr->currpos;
#endif
			if (ptr < &__parts[PARTEMIT+SECTCNT]) {
				offset[sectionnr] = ptr->currpos;
			}
		}
		else {
			i = n;
			ptr->cnt -= i;
			n = 0;
			while (i > 0) {
				*pn++ = *b++;
				i--;
			}
			ptr->pnow = pn;
		}
	}
	if (ptr->cnt == 0 || ptr->cnt == WBUFSIZ) {
		__wr_flush(ptr);
		m = n & ~(WBUFSIZ - 1);
		if (m != 0) {
			wr_bytes(ptr->fd, b, m);
			b += m;
			n &= WBUFSIZ - 1;
			ptr->currpos += m;
#ifdef OUTSEEK
			currpos = ptr->currpos;
#endif
			if (ptr < &__parts[PARTEMIT+SECTCNT]) {
				offset[sectionnr] = ptr->currpos;
			}
		}
		i = n;
		if (i != 0) {
			n = 0;
			pn = ptr->pnow;
			ptr->cnt -= i;
			while (i > 0) {
				*pn++ = *b++;
				i--;
			}
			ptr->pnow = pn;
		}
	}
}

static void BEGINSEEK(int p, long o)
	/* p = part number, o = offset in file */
{
	struct fil *ptr = &__parts[p];

#ifdef OUTSEEK
	ptr->fd = outfile;
	ptr->currpos = o;
#else
	ptr->currpos = fseek(ptr->fd, o, SEEK_SET);
#endif
	if (p >= PARTRELO) o = 0;	/* no attempt to align writes
					   for the time being */
	ptr->cnt = WBUFSIZ - ((int)o & (WBUFSIZ-1));
	ptr->pbegin = ptr->pbuf + (WBUFSIZ - ptr->cnt);
	ptr->pnow = ptr->pbegin;
}


/*
 * Open the output file according to the chosen strategy.
 */
int wr_open(const char *f)
{
	struct fil	*fdp;

	fclose(fopen(f,"wb"));
#ifdef OUTSEEK
	if ((outfile = fopen(f, "ab+")) == NULL)
		return 0;
	currpos = 0;
#else /* not OUTSEEK */
	for (fdp = &__parts[PARTEMIT]; fdp < &__parts[NPARTS]; fdp++)
		if ((fdp->fd = fopen(f, "wb+")) == NULL)
			return 0;
#endif /* not OUTSEEK */
	offcnt = 0;
	return 1;
}

void wr_close(void)
{
	struct fil *ptr;

	for (ptr = &__parts[PARTEMIT]; ptr < &__parts[NPARTS]; ptr++) {
		__wr_flush(ptr);
#ifndef OUTSEEK
		fclose(ptr->fd);
#endif /* not OUTSEEK */
		ptr->fd = NULL;
	}
#ifdef OUTSEEK
	fclose(outfile);
	outfile = -1;
#endif /* OUTSEEK */
}

void
wr_ohead(const struct outhead *head)
{
	{
		long off = OFF_RELO(*head);

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
	}

	{
		char buf[SZ_HEAD];

		char *c = &buf[0];

		put2(head->oh_magic, c);	c += 2;
		put2(head->oh_stamp, c);	c += 2;
		put2(head->oh_flags, c);	c += 2;
		put2(head->oh_nsect, c);	c += 2;
		put2(head->oh_nrelo, c);	c += 2;
		put2(head->oh_nname, c);	c += 2;
		put4(head->oh_nemit, c);	c += 4;
		put4(head->oh_nchar, c);
		OUTWRITE(PARTEMIT, buf, (long)SZ_HEAD);
	}
}

void
wr_sect(const struct outsect *sect, unsigned int cnt)
{
	{	unsigned int i = cnt;

		while (i--) {
			if (offcnt >= 1 && offcnt < SECTCNT) {
				BEGINSEEK(PARTEMIT+offcnt, sect->os_foff);
			}
			offset[offcnt++] = sect->os_foff;
			sect++;
		}
		sect -= cnt;
	}
	while (cnt)
	{
		char *c;
		unsigned int i;

		i = __parts[PARTEMIT].cnt/SZ_SECT;
		c = __parts[PARTEMIT].pnow;
		if (i > cnt) i = cnt;
		cnt -= i;
		__parts[PARTEMIT].cnt -= (i*SZ_SECT);
		while (i--) {
			put4(sect->os_base, c);	c += 4;
			put4(sect->os_size, c);	c += 4;
			put4(sect->os_foff, c);	c += 4;
			put4(sect->os_flen, c);	c += 4;
			put4(sect->os_lign, c);	c += 4;
			sect++;
		}
		__parts[PARTEMIT].pnow = c;
		if (cnt) {
			__wr_flush(&__parts[PARTEMIT]);
		}
	}
}

void
wr_outsect(int s)
	/* s = section number */
{
	struct fil *ptr = &__parts[PARTEMIT + getsect(sectionnr)];

	if (s != sectionnr && s >= (SECTCNT-1) && sectionnr >= (SECTCNT-1)) {
#ifdef OUTSEEK
		if (currpos != ptr->currpos) 
			currpos = fseek(ptr->fd, ptr->currpos, SEEK_SET);
#endif
		wr_bytes(ptr->fd, ptr->pbegin, (long)(ptr->pnow - ptr->pbegin));
		ptr->currpos += ptr->pnow - ptr->pbegin;
#ifdef OUTSEEK
		currpos = ptr->currpos;
#endif
		offset[sectionnr] = ptr->currpos;
		if (offset[s] != ptr->currpos) {
			ptr->currpos = fseek(ptr->fd, offset[s], SEEK_SET);
#ifdef OUTSEEK
			currpos = ptr->currpos;
#endif
		}
		ptr->cnt = WBUFSIZ - ((int)offset[s] & (WBUFSIZ-1));
		ptr->pbegin = ptr->pbuf + (WBUFSIZ - ptr->cnt);
		ptr->pnow = ptr->pbegin;
	}
	sectionnr = s;
}

/*
 * We don't have to worry about byte order here.
 */
void
wr_emit(const char *emit, long cnt)
{
	OUTWRITE(PARTEMIT + getsect(sectionnr) , emit, cnt);
}

void
wr_relo(const struct outrelo *relo, unsigned int cnt)
{

	while (cnt)
	{
		char *c;
		unsigned int i;

		i = __parts[PARTRELO].cnt/SZ_RELO;
		c = __parts[PARTRELO].pnow;
		if (i > cnt) i = cnt;
		cnt -= i;
		__parts[PARTRELO].cnt -= (i*SZ_RELO);
		while (i--) {
			put2(relo->or_type, c); c += 2;
			put2(relo->or_sect, c); c += 2;
			put2(relo->or_nami, c); c += 2;
			put4(relo->or_addr, c); c += 4;
			relo++;
		}
		__parts[PARTRELO].pnow = c;
		if (cnt) {
			__wr_flush(&__parts[PARTRELO]);
		}
	}
}

void
wr_name(const struct outname *name, unsigned int cnt)
{
	while (cnt)
	{
		char *c;
		unsigned int i;

		i = __parts[PARTNAME].cnt/SZ_NAME;
		c = __parts[PARTNAME].pnow;
		if (i > cnt) i = cnt;
		cnt -= i;
		__parts[PARTNAME].cnt -= (i*SZ_NAME);
		while (i--) {
			put4(name->on_foff, c);	c += 4;
			put2(name->on_type, c);	c += 2;
			put2(name->on_desc, c);	c += 2;
			put4(name->on_valu, c); c += 4;
			name++;
		}
		__parts[PARTNAME].pnow = c;
		if (cnt) __wr_flush(&__parts[PARTNAME]);
	}
}

void
wr_string(const char *addr, long len)
{
	OUTWRITE(PARTCHAR, addr, len);
}

#ifdef SYMDBUG

void
wr_dbug(buf, size)
	char		*buf;
	long		size;
{
	OUTWRITE(PARTDBUG, buf, size);
}

#endif /* SYMDBUG */
