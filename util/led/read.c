#ifndef lint
static char rcsid[] = "$Header$";
#endif

/*
 * Routines to read in the various parts of the object file.
 */

#include "../../h/arch.h"
#include "../../h/out.h"
#include "../../h/ranlib.h"
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

read1(fd, val)
char *val ; {
	if ( read(fd, val, 1)!=1 ) return 0 ;
	return 1 ;
}
read2(fd, val)
int *val ; {
	char rch[2] ;
	if ( read(fd, rch, 2)!=2 ) return 0 ;
	*val= (rch[0]&0377) + ((rch[1]&0377)<<8) ;
	return 1 ;
}
read4(fd, val)
long *val ; {
	int v1,v2 ;
	if ( !read2(fd, &v1) ) return 0 ;
	if ( !read2(fd, &v2) ) return 0 ;
	*val = ((long)v1<<16) + (unsigned)v2 ;
	return 1 ;
}


read_arhdr(arhdr)
	register struct ar_hdr	*arhdr;
{
	if ( read(infile,arhdr->ar_name,sizeof arhdr->ar_name)!=
	     sizeof arhdr->ar_name) {
		goto peof ;
	}
	if ( !read4(infile,&arhdr->ar_date) ) goto peof ;
	if ( !read1(infile,&arhdr->ar_uid) ) goto peof ;
	if ( !read1(infile,&arhdr->ar_gid) ) goto peof ;
	if ( !read2(infile,&arhdr->ar_mode) ) goto peof ;
	if ( !read4(infile,&arhdr->ar_size) ) goto peof ;
	return ;
peof:
	fatal("Prematute EOF") ;
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
