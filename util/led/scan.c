/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef SYMDBUG
#include <sys/types.h>
#include <sys/stat.h>
#endif /* SYMDBUG */
#include "arch.h"
#include "out.h"
#include "ranlib.h"
#include "const.h"
#include "assert.h"
#include "memory.h"
#include "scan.h"
#include "debug.h"

#define READ	0

#define IND_EMIT(x)	(IND_CHAR(x) + (ind_t)align((x).oh_nchar))
#define IND_RELO(x)	(IND_EMIT(x) + (x).oh_nsect * sizeof(ind_t))
#ifdef SYMDBUG
#define IND_DBUG(x)	(IND_RELO(x) + sizeof(ind_t))
#endif /* SYMDBUG */

extern long	lseek();
extern char 	*core_alloc();
extern bool	incore;
extern int	infile;
extern int	passnumber;

char		*archname;	/* Name of archive, if reading from archive. */
char		*modulname;	/* Name of object module. */
#ifdef SYMDBUG
long		objectsize;
#endif /* SYMDBUG */

static long	align();
static char	*modulbase;
static long	modulsize();
static		scan_modul();
static bool	all_alloc();
static bool	direct_alloc();
static bool	indirect_alloc();
static bool	putemitindex();
static bool	putreloindex();
#ifdef SYMDBUG
static bool	putdbugindex();
#endif /* SYMDBUG */
static		get_indirect();
static		read_modul();

/*
 * Open the file with name `filename' (if necessary) and examine the first
 * few bytes to see if it's a plain file or an archive.
 * In case of a plain file, the file pointer is repositioned after the
 * examination. Otherwise it is at the beginning of the table of contents.
 */
int
getfile(filename)
	char		*filename;
{
	unsigned int	rd_unsigned2();
	struct ar_hdr	archive_header;
	unsigned short	magic_number;
#ifdef SYMDBUG
	struct stat	statbuf;
	extern int	fstat();
#endif /* SYMDBUG */

	archname = (char *)0;
	modulname = (char *)0;

	if (passnumber == FIRST || !incore) {
		if ((infile = open(filename, READ)) < 0)
			fatal("can't read %s", filename);
		magic_number = rd_unsigned2(infile);
	} else {
		modulbase = modulptr((ind_t)0);
		magic_number = *(unsigned short *)modulbase;
	}

	switch (magic_number) {
	case O_MAGIC:
#ifdef SYMDBUG
		if (passnumber == FIRST || !incore) {
			if (fstat(infile, &statbuf) < 0)
				fatal("cannot stat %s", filename);
			objectsize = statbuf.st_size;
		}
#endif /* SYMDBUG */
		seek((long)0);
		modulname = filename;
		return PLAIN;
	case ARMAG:
	case AALMAG:
		archname = filename;
		if (passnumber == FIRST) {
			rd_arhdr(infile, &archive_header);
			if (strcmp(archive_header.ar_name, SYMDEF))
				fatal("%s: no table of contents", filename);
		} else if (incore) {
			modulbase += sizeof(int);
			core_position += sizeof(int);
		}
		return ARCHIVE;
	default:
		fatal("%s: wrong magic number", filename);
	}
	/* NOTREACHED */
}

/* ARGSUSED */
closefile(filename)
	char	*filename;
{
	if (passnumber == FIRST || !incore)
		close(infile);
}

get_archive_header(archive_header)
	register struct ar_hdr	*archive_header;
{
	if (passnumber == FIRST || !incore) {
		rd_arhdr(infile, archive_header);
	} else {
		/* Copy structs. */
		*archive_header = *(struct ar_hdr *)modulbase;
		modulbase += int_align(sizeof(struct ar_hdr));
		core_position += int_align(sizeof(struct ar_hdr));
	}
#ifdef SYMDBUG
	objectsize = archive_header.ar_size;
#endif /* SYMDBUG */
}

get_modul()
{
	if (passnumber == FIRST) {
		rd_fdopen(infile);
		scan_modul();
	} else if (!incore) {
		rd_fdopen(infile);
		read_modul();
	}
}

/*
 * Read module from the current file. If it doesn't fit into core, the strategy
 * to keep everything in core is abandoned, but we will always put the header,
 * the section table, and the name and string table into core.
 */
static
scan_modul()
{
	bool		space;
	struct outhead	*head;
	struct outsect	*sect;

	space = all_alloc();
	head = (struct outhead *)modulptr(IND_HEAD);
	if (space) {
		sect = (struct outsect *)modulptr(IND_SECT(*head));
		get_indirect(head, sect);
	}
	rd_name((struct outname *)modulptr(IND_NAME(*head)), head->oh_nname);
	rd_string((char *)modulptr(IND_CHAR(*head)), head->oh_nchar);
#ifdef SYMDBUG
	if (space) {
		get_dbug(*(ind_t *)modulptr(IND_DBUG(*head)),
			 ojectsize - OFF_DBUG(*head)
		);
	}
#endif /* SYMDBUG */
}

/*
 * Allocate space for and read in the header and section table.
 * First get the header. With this we can determine what to allocate
 * for the rest of the module, and with the rest we can determine what
 * to allocate for the section contents.
 * If possible, allocate space for the rest of the module. Return whether
 * this was possible.
 */
static bool
all_alloc()
{
	struct outhead	head;
	extern ind_t	hard_alloc();

	if (hard_alloc(ALLOMODL, (long)sizeof(struct outhead)) == BADOFF)
		fatal("no space for module header");
	rd_ohead((struct outhead *)modulptr(IND_HEAD));
	/*
	 * Copy the header because we need it so often.
	 */
	head = *(struct outhead *)modulptr(IND_HEAD);
	return direct_alloc(&head) && indirect_alloc(&head);
}

/*
 * Allocate space for the rest of the direct bytes.
 * First allocate the section table and read it in, then allocate the rest
 * and return whether this succeeded.
 */
static bool
direct_alloc(head)
	struct outhead	*head;
{
	ind_t		sectindex = IND_SECT(*head);
	register struct outsect *sects;
	unsigned short	nsect = head->oh_nsect;
	long		size, rest;
	extern ind_t	hard_alloc();
	extern ind_t	alloc();

#ifdef SYMDBUG
	rest = nsect * sizeof(ind_t) + sizeof(ind_t) + sizeof(ind_t);
#else /* SYMDBUG */
	rest = nsect * sizeof(ind_t) + sizeof(ind_t);
#endif /* SYMDBUG */
	/*
	 * We already allocated space for the header, we now need
	 * the section, name an string table.
	 */
	size = modulsize(head) - sizeof(struct outhead) - rest;
	if (hard_alloc(ALLOMODL, size) == BADOFF)
		fatal("no space for module");
	rd_sect(sects = ((struct outsect *)modulptr(sectindex)), nsect);
	while (nsect--) {
		if (sects->os_lign > 1) {
			sects->os_size += sects->os_lign - 1;
			sects->os_size -= sects->os_size % sects->os_lign;
		}
		sects++;
	}

	return incore && alloc(ALLOMODL, rest) != BADOFF;
}

/*
 * Allocate space for the indirectly accessed pieces: the section contents and
 * the relocation table, and put their indices in the right place.
 */
static bool
indirect_alloc(head)
	struct outhead	*head;
{
	register int	allopiece;
	unsigned short	nsect = head->oh_nsect;
	unsigned short	nrelo = head->oh_nrelo;
	ind_t		sectindex = IND_SECT(*head);
	ind_t		emitoff = IND_EMIT(*head);
	ind_t		relooff = IND_RELO(*head);
#ifdef SYMDBUG
	ind_t		dbugoff = IND_DBUG(*head);
	extern long	objectsize;
	long		dbugsize = objectsize - OFF_DBUG(*head);
#endif /* SYMDBUG */

	assert(incore);
	for (allopiece = ALLOEMIT; allopiece < ALLOEMIT + nsect; allopiece++) {
		if (!putemitindex(sectindex, emitoff, allopiece))
			return FALSE;
		sectindex += sizeof(struct outsect);
		emitoff += sizeof(ind_t);
	}
#ifdef SYMDBUG
	return	putreloindex(relooff, (long)nrelo * sizeof(struct outrelo))
		&&
		putdbugindex(dbugoff, dbugsize);
#else /* SYMDBUG */
	return putreloindex(relooff, (long)nrelo * sizeof(struct outrelo));
#endif /* SYMDBUG */
}

/*
 * Allocate space for the contents of the section of which the table entry is
 * at offset `sectindex'. Put the offset of the allocated piece at offset
 * `emitoff'.
 */
static bool
putemitindex(sectindex, emitoff, allopiece)
	ind_t		sectindex;
	ind_t		emitoff;
	int		allopiece;
{
	long		flen;
	ind_t		emitindex;
	extern ind_t	alloc();
	static long	zeros[MAXSECT];
	register long	 zero  = zeros[allopiece - ALLOEMIT];

	/*
	 * Notice that "sectindex" is not a section number!
	 * It contains the offset of the section from the beginning
	 * of the module. Thus, it cannot be used to index "zeros".
	 * AIAIAIAIA
	 */

	flen = ((struct outsect *)modulptr(sectindex))->os_flen;
	if (flen && zero) {
		if ((emitindex = alloc(allopiece, zero)) != BADOFF){
			register char *p = address(allopiece, emitindex);

			debug("Zeros %ld\n", zero, 0,0,0);
			while (zero--) *p++ = 0;
		}
		else	 return FALSE;
		zero = 0;
	}
	zeros[allopiece - ALLOEMIT] =
	 zero + ((struct outsect *) modulptr(sectindex))->os_size - flen;
	if ((emitindex = alloc(allopiece, flen)) != BADOFF) {
		*(ind_t *)modulptr(emitoff) = emitindex;
		return TRUE;
	}
	return FALSE;
}

/*
 * Allocate space for a relocation table with `nrelobytes' bytes, and put the
 * offset at `relooff'.
 */
static bool
putreloindex(relooff, nrelobytes)
	ind_t		relooff;
	long		nrelobytes;
{
	ind_t		reloindex;
	extern ind_t	alloc();

	if ((reloindex = alloc(ALLORELO, nrelobytes)) != BADOFF) {
		*(ind_t *)modulptr(relooff) = reloindex;
		return TRUE;
	}
	return FALSE;
}
#ifdef SYMDBUG
/*
 * Allocate space for debugging information and put the offset at `dbugoff'.
 */
static bool
putdbugindex(dbugoff, ndbugbytes)
	ind_t		relooff;
	long		ndbugbytes;
{
	ind_t		dbugindex;
	extern ind_t	alloc();

	if ((dbugindex = alloc(ALLODBUG, ndbugbytes)) != BADOFF) {
		*(ind_t *)modulptr(dbugoff) = dbugindex;
		return TRUE;
	}
	return FALSE;
}
#endif /* SYMDBUG */

/*
 * Compute addresses and read in. Remember that the contents of the sections
 * and also the relocation table are accessed indirectly.
 */
static
get_indirect(head, sect)
	struct outhead	*head;		/* not register! Won't compile on
					   SCO Xenix 386 if it is!
					*/
	register struct outsect	*sect;
{
	register ind_t		*emitindex;
	register int		nsect;
	register int		piece;
	ind_t			*reloindex;

	emitindex = (ind_t *)modulptr(IND_EMIT(*head));
	piece = ALLOEMIT;
	for (nsect = 0; nsect < head->oh_nsect; nsect++) {
		rd_outsect(nsect);
		rd_emit(address(piece, *emitindex), sect->os_flen);
		piece++; emitindex++; sect++;
	}
	reloindex = (ind_t *)modulptr(IND_RELO(*head));
	rd_relo((struct outrelo *)address(ALLORELO, *reloindex),
		head->oh_nrelo
	);
}

/*
 * Set the file pointer at `pos'.
 */
seek(pos)
	long		pos;
{
	if (passnumber == FIRST || !incore)
		lseek(infile, pos, 0);
}

/*
 * A file pointer is advanced automatically when reading, a char pointer
 * is not. That's why we do it here. If we don't keep everything in core,
 * we give the space allocated for a module back.
 */
skip_modul(head)
	struct outhead	*head;
{
	register ind_t	skip = modulsize(head);

	if (incore) {
		core_position += int_align(skip);
		if (passnumber == SECOND)
			modulbase += int_align(skip);
	} else {
		dealloc(ALLOMODL);
		core_position = (ind_t)0;
	}
}

/*
 * Read in what we need in pass 2, because we couldn't keep it in core.
 */
static
read_modul()
{
	struct outhead	*head;
	register struct outsect	*sects;
	struct outname	*names;
	char		*chars;
	ind_t		sectindex, nameindex, charindex;
	unsigned short	nsect, nname;
	long		size;
	long		nchar;
	extern ind_t	hard_alloc();

	assert(passnumber == SECOND);
	assert(!incore);
	if (hard_alloc(ALLOMODL, (long)sizeof(struct outhead)) == BADOFF)
		fatal("no space for module header");
	head = (struct outhead *)modulptr(IND_HEAD);
	rd_ohead(head);
	nsect = head->oh_nsect; sectindex = IND_SECT(*head);
	nname = head->oh_nname; nameindex = IND_NAME(*head);
	nchar = head->oh_nchar; charindex = IND_CHAR(*head);
#ifdef SYMDBUG
	size = modulsize(head) - (nsect * sizeof(ind_t) + 2 * sizeof(ind_t));
#else /* SYMDBUG */
	size = modulsize(head) - (nsect * sizeof(ind_t) + sizeof(ind_t));
#endif /* SYMDBUG */
	if (hard_alloc(ALLOMODL, size) == BADOFF)
		fatal("no space for module");

	sects = (struct outsect *)modulptr(sectindex);
	names = (struct outname *)modulptr(nameindex);
	chars = modulptr(charindex);

	rd_sect(sects, nsect);
	while (nsect--) {
		if (sects->os_lign > 1) {
			sects->os_size += sects->os_lign - 1;
			sects->os_size -= sects->os_size % sects->os_lign;
		}
		sects++;
	}
	rd_name(names, nname);
	rd_string(chars, nchar);
}

/*
 * Align `size' to a multiple of the size of a double.
 * This is assumed to be a power of 2.
 */
static long
align(size)
	register long	size;
{
	return (size + (sizeof(double) - 1)) & ~(int)(sizeof(double) - 1);
}

/*
 * Compute how many DIRECT bytes must be allocated for a module of which the
 * header is pointed to by `head':
 *	0. the header,
 * 	1. the section table,
 *	2. the name table,
 *	3. the string table,
 *	4. for each section the offset of its contents,
 *	5. the offset of the relocation table.
#ifdef SYMDBUG
 *	6. the offset of the debugging information.
#endif
 */
static long
modulsize(head)
	register struct outhead	*head;
{
	return	sizeof(struct outhead) +			/* 0 */
		head->oh_nsect * sizeof(struct outsect) +	/* 1 */
		head->oh_nname * sizeof(struct outname) +	/* 2 */
		align(head->oh_nchar) +				/* 3 */
		head->oh_nsect * sizeof(ind_t) +		/* 4 */
#ifdef SYMDBUG
		sizeof(ind_t) +					/* 5 */
		sizeof(ind_t);					/* 6 */
#else /* SYMDBUG */
		sizeof(ind_t);					/* 5 */
#endif /* SYMDBUG */
}

/* ------------------------------------------------------------------------- */

/*
 * Walk through the relocation table of the current module. We must either walk
 * through core or through file. Startrelo() should be called first.
 */

static struct outrelo	*walkrelo;
static unsigned short cnt_relos;
static unsigned short relind;
#define _RELSIZ	64

startrelo(head)
	register struct outhead	*head;
{
	ind_t		reloindex;

	if (incore) {
		reloindex = *(ind_t *)(modulbase + IND_RELO(*head));
		walkrelo = (struct outrelo *)address(ALLORELO, reloindex);
	}
	else {
		relind = _RELSIZ;
		rd_rew_relos(head);
		cnt_relos = head->oh_nrelo;
	}
}

struct outrelo *
nextrelo()
{
	static struct outrelo	relobuf[_RELSIZ];

	if (incore)
		return walkrelo++;

	if (relind == _RELSIZ) {
		unsigned int i = cnt_relos >= _RELSIZ ? _RELSIZ : cnt_relos;

		cnt_relos -= i;
		rd_relo(relobuf, i);
		relind = 0;
	}
	return &relobuf[relind++];
}

/* ------------------------------------------------------------------------- */

/*
 * Get the section contents in core of which the describing struct has index
 * `sectindex'. `Head' points to the header of the module.
 */
char *
getemit(head, sects, sectindex)
	struct outhead	*head;
	struct outsect	*sects;
	int		sectindex;
{
	char		*ret;
	ind_t		off;
	extern char	*core_alloc();

	if (!incore) {
		ret = core_alloc(ALLOMODL, sects[sectindex].os_flen);
		if (ret == (char *)0)
			return 0;
		rd_outsect(sectindex);
		rd_emit(ret, sects[sectindex].os_flen);
		return ret;
	}
	/*
	 * We have an offset in the contents of the final output
	 * "file" where normally the contents would be.
	 */
	off = *((ind_t *)(modulbase + IND_EMIT(*head)) + sectindex);
	return address(ALLOEMIT + sectindex, off);
}

char *
getblk(totalsz, pblksz, sectindex)
	long	totalsz;
	long	*pblksz;
	int	sectindex;
{
	char	*ret;
	long	sz = (1L << 30);

	assert(!incore);

	while (sz >= totalsz) sz >>= 1;
	while (sz) {
		ret = core_alloc(ALLOMODL, sz);
		if (ret != (char *) 0) {
			rd_outsect(sectindex);
			*pblksz = sz;
			return ret;
		}
		sz >>= 1;
	}
	fatal("no space for section contents");
	return (char *) 0;
}

endemit(emit)
	char	*emit;
{
	core_free(ALLOMODL, emit);
}
