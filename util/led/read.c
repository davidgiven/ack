#ifndef lint
static char rcsid[] = "$Header$";
#endif

/*
 * Routines to read in the various parts of the object file.
 */

#include "arch.h"
#include "out.h"
#include "ranlib.h"
#include "const.h"
#include "assert.h"

int	infile;	/* The current input file. */

extern bool	bytes_reversed;
extern bool	words_reversed;

ushort
getushort()
{
	ushort	ubuf;

	if (read(infile, (char *)&ubuf, 2) != 2)
		fatal("premature EOF");
	if (bytes_reversed)
		swap((char *)&ubuf, "2");
	return ubuf;
}

long
getlong()
{
	long	lbuf;

	if (read(infile, (char *)&lbuf, 4) != 4)
		fatal("premature EOF");
	if (bytes_reversed || words_reversed)
		swap((char *)&lbuf, "4");
	return lbuf;
}

read_head(head)
	register struct outhead	*head;
{
	if (read(infile, (char *)head, SZ_HEAD) != SZ_HEAD)
		fatal("premature EOF");
	if (bytes_reversed || words_reversed)
		swap((char *)head, SF_HEAD);
	if (BADMAGIC(*head))
		fatal("bad magic number");
}

/*
 * Someone inadvertently misaligned a long, thereby creating a hole.
 * Therefore we can't read the header in one chunk.
 */
read_arhdr(arhdr)
	register struct ar_hdr	*arhdr;
{
	if (read(infile, (char *)arhdr, 14) != 14)
		fatal("premature EOF");
	if (read(infile, (char *)&arhdr->ar_date, SZ_ARCH - 14) != SZ_ARCH - 14)
		fatal("premature EOF");
	if (bytes_reversed || words_reversed)
		swap((char *)&arhdr->ar_date, SF_ARCH);
}

read_table(ran, cnt)
	register struct ranlib	*ran;
	register long		cnt;
{
	read_char((char *)ran, cnt * SZ_RAN);
	if (bytes_reversed || words_reversed)
		while (cnt--)
			swap((char *)ran++, SF_RAN);
}

read_sect(sect, cnt)
	register struct outsect	*sect;
	register ushort		cnt;
{
	if (read(infile, (char *)sect, cnt * SZ_SECT) != cnt * SZ_SECT)
		fatal("premature EOF");
	if (bytes_reversed || words_reversed)
		while (cnt--)
			swap((char *)sect++, SF_SECT);
}

read_emit(emit, cnt)
	register char		*emit;
	register long		cnt;
{
	read_char(emit, cnt);
}

read_relo(relo, cnt)
	register struct outrelo	*relo;
	register ushort		cnt;
{
	read_char((char *)relo, (long)cnt * SZ_RELO);
	if (bytes_reversed || words_reversed)
		while (cnt--)
			swap((char *)relo++, SF_RELO);
}

read_name(name, cnt)
	register struct outname	*name;
	register ushort		cnt;
{
	read_char((char *)name, (long)cnt * SZ_NAME);
	if (bytes_reversed || words_reversed)
		while (cnt--)
			swap((char *)name++, SF_NAME);
}

/*
 * We don't have to worry about byte order here.
 */
read_char(string, cnt)
	register char	*string;
	register long	cnt;
{
	register int	n;

	while (cnt) {
		n = cnt >= MAXCHUNK ? MAXCHUNK : cnt;
		if (read(infile, string, n) != n)
			fatal("premature EOF");
		string += n;
		cnt -= n;
	}
}
