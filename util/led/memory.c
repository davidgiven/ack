#ifndef lint
static char rcsid[] = "$Header$";
#endif

/*
 * Memory manager. Memory is divided into NMEMS pieces. There is a struct
 * for each piece telling where it is, how many bytes are used, and how may
 * are left. If a request for core doesn't fit in the left bytes, an sbrk()
 * is done and pieces after the one that requested the growth are moved up.
 */

#include "out.h"
#include "const.h"
#include "assert.h"
#include "debug.h"
#include "memory.h"

struct memory	mems[NMEMS];

bool	incore = TRUE;	/* TRUE while everything can be kept in core. */
ind_t	core_position = (ind_t)0;	/* Index of current module. */

#define AT_LEAST	2	/* See comment about string areas. */

/*
 * Initialize some pieces of core. We hope that this will be our last
 * real allocation, meaning we've made the right choices.
 */
init_core()
{
	register char		*base;
	register ind_t		total_size;
	register struct memory	*mem;
	extern char		*sbrk();

#include "mach.c"

	total_size = (ind_t)0;	/* Will accumulate the sizes. */
	base = sbrk(0);		/* First free. */
	for (mem = mems; mem < &mems[NMEMS]; mem++) {
		mem->mem_base = base;
		mem->mem_full = (ind_t)0;
		base += mem->mem_left;	/* Each piece will start after prev. */
		total_size += mem->mem_left;
	}
	/*
	 * String areas are special-cased. The first byte is unused as a way to
	 * distinguish a name without string from a name which has the first
	 * string in the string area.
	 */
	if (mems[ALLOLCHR].mem_left == 0)
		total_size += 1;
	else
		mems[ALLOLCHR].mem_left -= 1;
	if (mems[ALLOGCHR].mem_left ==  0)
		total_size += 1;
	else
		mems[ALLOGCHR].mem_left -= 1;
	mems[ALLOLCHR].mem_full = 1;
	mems[ALLOGCHR].mem_full = 1;

	if (total_size != (int)total_size || (int)sbrk((int)total_size) == -1) {
		incore = FALSE;	/* In core strategy failed. */
		if ((int)sbrk(AT_LEAST) == -1)
			fatal("no core at all");
	}
}

/*
 * Allocate an extra block of `incr' bytes and move all pieces with index
 * higher than `piece' up with the size of the block. Return whether the
 * allocate succeeded.
 */
static bool
move_up(piece, incr)
	register int		piece;
	register ind_t		incr;
{
	register struct memory	*mem;
	extern char		*sbrk();

	debug("move_up(%d, %d)\n", piece, (int)incr, 0, 0);
	if (incr != (int)incr || (int)sbrk((int)incr) == -1)
		return FALSE;

	for (mem = &mems[NMEMS - 1]; mem > &mems[piece]; mem--)
		copy_up(mem, incr);

	mems[piece].mem_left += incr;
	return TRUE;
}

extern int	passnumber;

/*
 * This routine is called if `piece' needs `incr' bytes and the system won't
 * give them. We first steal the free bytes of all lower pieces and move them
 * and `piece' down. If that doesn't give us enough bytes, we steal the free
 * bytes of all higher pieces and move them up. We return whether we have
 * enough bytes, the first or the second time.
 */
static bool
compact(piece, incr)
	register int		piece;
	register ind_t		incr;
{
	register ind_t		gain;
	register struct memory	*mem;

	debug("compact(%d, %d)\n", piece, (int)incr, 0, 0);
	gain = mems[0].mem_left;
	mems[0].mem_left = (ind_t)0;
	for (mem = &mems[1]; mem <= &mems[piece]; mem++) {
		/* Here memory is inserted before a piece. */
		assert(passnumber == FIRST || gain == (ind_t)0);
		copy_down(mem, gain);
		gain += mem->mem_left;
		mem->mem_left = (ind_t)0;
	}
	/*
	 * Note that we already added the left bytes of the piece we want to
	 * enlarge to `gain'.
	 */
	if (gain < incr) {
		register ind_t	up = (ind_t)0;

		for (mem = &mems[NMEMS - 1]; mem > &mems[piece]; mem--) {
			/* Here memory is appended after a piece. */
			up += mem->mem_left;
			copy_up(mem, up);
			mem->mem_left = (ind_t)0;
		}
		gain += up;
	}
	mems[piece].mem_left = gain;
	return gain >= incr;
}

/*
 * The bytes of `mem' must be moved `dist' down in the address space.
 * We copy the bytes from low to high, because the tail of the new area may
 * overlap with the old area, but we do not want to overwrite them before they
 * are copied.
 */
static
copy_down(mem, dist)
	register struct memory	*mem;
	ind_t			dist;
{
	register char		*old;
	register char		*new;
	register ind_t		size;

	size = mem->mem_full;
	old = mem->mem_base;
	new = old - dist;
	mem->mem_base = new;
	while (size--)
		*new++ = *old++;
}

/*
 * The bytes of `mem' must be moved `dist' up in the address space.
 * We copy the bytes from high to low, because the tail of the new area may
 * overlap with the old area, but we do not want to overwrite them before they
 * are copied.
 */
static
copy_up(mem, dist)
	register struct memory	*mem;
	ind_t			dist;
{
	register char		*old;
	register char		*new;
	register ind_t		size;

	size = mem->mem_full;
	old = mem->mem_base + size;
	new = old + dist;
	while (size--)
		*--new = *--old;
	mem->mem_base = new;
}

/*
 * Add `size' bytes to the bytes already allocated for `piece'. If it has no
 * free bytes left, ask them from memory or, if that fails, from the free
 * bytes of other pieces. The offset of the new area is returned. No matter
 * how many times the area is moved, because of another allocate, this offset
 * remains valid.
 */
ind_t
alloc(piece, size)
	register int		piece;
	register long		size;
{
	register ind_t		incr = 0;
	register ind_t		left = mems[piece].mem_left;
	register ind_t		full = mems[piece].mem_full;

	assert(passnumber == FIRST || (!incore && piece == ALLOMODL));
	if (size == (long)0)
		return full;
	if (size != (ind_t)size)
		return BADOFF;

	while (left + incr < size)
		incr += INCRSIZE;

	if (incr == 0 || move_up(piece, incr) || compact(piece, incr)) {
		mems[piece].mem_full += size;
		mems[piece].mem_left -= size;
		return full;
	} else {
		incore = FALSE;
		return BADOFF;
	}
}

/*
 * Same as alloc() but for a piece which really needs it. If the first
 * attempt fails, release the space occupied by other pieces and try again.
 */
ind_t
hard_alloc(piece, size)
	register int	piece;
	register long	size;
{
	register ind_t	ret;
	register int	i;

	if (size != (ind_t)size)
		return BADOFF;
	if ((ret = alloc(piece, size)) != BADOFF)
		return ret;

	/*
	 * Deallocate what we don't need.
	 */
	for (i = 0; i < NMEMS; i++) {
		switch (i) {
		case ALLOGLOB:
		case ALLOGCHR:
		case ALLOSYMB:
		case ALLOARCH:
		case ALLOMODL:
			break;	/* Do not try to deallocate this. */
		default:
			dealloc(i);
			break;
		}
	}
	free_saved_moduls();

	return alloc(piece, size);
}

/*
 * We don't need the previous modules, so we put the current module
 * at the start of the piece allocated for module contents, thereby
 * overwriting the saved modules, and release its space.
 */
static
free_saved_moduls()
{
	register ind_t		size;
	register char		*old, *new;
	register struct memory	*mem = &mems[ALLOMODL];

	size = mem->mem_full - core_position;
	new = mem->mem_base;
	old = new + core_position;
	while (size--)
		*new++ = *old++;
	mem->mem_full -= core_position;
	mem->mem_left += core_position;
	core_position = (ind_t)0;
}

/*
 * The piece of memory with index `piece' is no longer needed.
 * We take care that it can be used by compact() later, if needed.
 */
dealloc(piece)
	register int		piece;
{
	/*
	 * Some pieces need their memory throughout the program.
	 */
	assert(piece != ALLOGLOB);
	assert(piece != ALLOGCHR);
	assert(piece != ALLOSYMB);
	assert(piece != ALLOARCH);
	mems[piece].mem_left += mems[piece].mem_full;
	mems[piece].mem_full = (ind_t)0;
}

char *
core_alloc(piece, size)
	register int	piece;
	register long	size;
{
	register ind_t	off;

	if ((off = alloc(piece, size)) == BADOFF)
		return (char *)0;
	return address(piece, off);
}

/*
 * Reset index into piece of memory for modules and
 * take care that the allocated pieces will not be moved.
 */
freeze_core()
{
	register int	i;

	core_position = (ind_t)0;

	if (incore)
		return;

	for (i = 0; i < NMEMS; i++) {
		switch (i) {
		case ALLOGLOB:
		case ALLOGCHR:
		case ALLOSYMB:
		case ALLOARCH:
			break;	/* Do not try to deallocate this. */
		default:
			dealloc(i);
			break;
		}
	}
	compact(NMEMS - 1, (ind_t)0);
}

/* ------------------------------------------------------------------------- */

extern bool	bytes_reversed;
extern bool	words_reversed;

/*
 * To transform the various pieces of the output in core to the file format,
 * we must order the bytes in the ushorts and longs as ACK prescribes.
 */
write_bytes()
{
	ushort			nsect, nrelo;
	long			offchar;
	int			fd;
	register struct memory	*mem;
	extern ushort		NLocals, NGlobals;
	extern long		NLChars, NGChars;
	extern int		flagword;
	extern struct outhead	outhead;
	extern struct outsect	outsect[];
	extern char		*outputname;

	nsect = outhead.oh_nsect;
	nrelo = outhead.oh_nrelo;
	offchar = OFF_CHAR(outhead);

	if (bytes_reversed || words_reversed) {
		swap((char *)&outhead, SF_HEAD);
		sectswap(outsect, nsect);
		reloswap(nrelo);
	}
	/*
	 * We allocated two areas: one for local and one for global names.
	 * Also, we used another kind of on_foff than on file.
	 * At the end of the global area we have put the section names.
	 */
	if (!(flagword & SFLAG)) {
		namecpy((struct outname *)mems[ALLOLOCL].mem_base,
			NLocals,
			offchar
		);
		namecpy((struct outname *)mems[ALLOGLOB].mem_base,
			NGlobals + nsect,
			offchar + NLChars
		);
	}
	if ((fd = creat(outputname, 0666)) < 0)
		fatal("can't create %s", outputname);
	/*
	 * These pieces must always be written.
	 */
	writelong(fd, (char *)&outhead, (ind_t)SZ_HEAD);
	writelong(fd, (char *)outsect, (ind_t)nsect * SZ_SECT);
	for (mem = &mems[ALLOEMIT]; mem < &mems[ALLORELO]; mem++)
		writelong(fd, mem->mem_base, mem->mem_full);
	/*
	 * The rest depends on the flags.
	 */
	if (flagword & RFLAG)
		writelong(fd, mems[ALLORELO].mem_base, mems[ALLORELO].mem_full);
	if (!(flagword & SFLAG)) {
		writelong(fd, mems[ALLOLOCL].mem_base, mems[ALLOLOCL].mem_full);
		writelong(fd, mems[ALLOGLOB].mem_base, mems[ALLOGLOB].mem_full);
		writelong(fd, mems[ALLOLCHR].mem_base + 1, (ind_t)NLChars);
		writelong(fd, mems[ALLOGCHR].mem_base + 1, (ind_t)NGChars);
#ifdef SYMDBUG
		writelong(fd, mems[ALLODBUG].mem_base, mems[ALLODBUG].mem_full);
#endif SYMDBUG
	}
	close(fd);
}

static
writelong(fd, base, size)
	register int	fd;
	register char	*base;
	register ind_t	size;
{
	register int	chunk;

	while (size) {
		chunk = size > (ind_t)MAXCHUNK ? MAXCHUNK : size;
		write(fd, base, chunk);
		size -= chunk;
		base += chunk;
	}
}

static
sectswap(sect, nsect)
	register struct outsect	*sect;
	register ushort		nsect;
{
	while (nsect--)
		swap((char *)sect++, SF_SECT);
}

static
reloswap(nrelo)
	register ushort		nrelo;
{
	register struct outrelo	*relo;

	relo = (struct outrelo *)mems[ALLORELO].mem_base;
	while (nrelo--)
		swap((char *)relo++, SF_RELO);
}

static
namecpy(name, nname, offchar)
	register struct outname	*name;
	register ushort		nname;
	register long		offchar;
{
	while (nname--) {
		if (name->on_foff)
			name->on_foff += offchar - 1;
		if (bytes_reversed || words_reversed)
			swap((char *)name, SF_NAME);
		name++;
	}
}
