/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Header$";
#endif

/*
 * If everything is kept in core, we must save some things for the second pass.
 */

#include <arch.h>
#include <out.h>
#include "const.h"
#include "assert.h"
#include "memory.h"

extern bool	incore;
extern char	*core_alloc();

savemagic()
{
	register char	*p;

	if (!incore)
		return;

	if ((p = core_alloc(ALLOMODL, (long)sizeof(int))) != (char *)0) {
		*(unsigned short *)p = AALMAG;
		core_position += sizeof(int);
	}
}

savehdr(hdr)
	struct ar_hdr	*hdr;
{
	register char	*p;

	if (!incore)
		return;

	if ((p=core_alloc(ALLOMODL,(long)sizeof(struct ar_hdr)))!=(char *)0) {
		*(struct ar_hdr *)p = *hdr;
		core_position += int_align(sizeof(struct ar_hdr));
	}
}

long	NLChars = 0;	/* Size of string area for local names. */
long	NGChars = 0;	/* Idem for global names. */

/*
 * Put the string in cp into the block allocated for the string area.
 * Return its offset in this area. We don't use the first char of the string
 * area, so that empty strings can be distinguished from the first string.
 */
ind_t
savechar(piece, off)
	register int	piece;
	register ind_t	off;
{
	register long	len;
	register ind_t	newoff;
	extern int	strlen();
	extern ind_t	alloc();
	extern ind_t	hard_alloc();
	extern char	*strcpy();

	if (off == (ind_t)0)
		return 0;

	len = strlen(modulptr(off)) + 1;
	if (piece == ALLOLCHR) {
		NLChars += len;
		if (!incore || (newoff = alloc(piece, len)) == BADOFF)
			return BADOFF;
	} else {
		assert(piece == ALLOGCHR);
		NGChars += len;
		if ((newoff = hard_alloc(piece, len)) == BADOFF)
			return BADOFF;
	}
	strcpy(address(piece, newoff), modulptr(off));
	return newoff;
}

/*
 * Put the local in `name' in the piece allocated for local names that must
 * be saved. `Name' points to a private copy, so will not become invalid after
 * allocation, but the string of which name->on_foff is the offset may be
 * destroyed, so we save that first.
 */
savelocal(name)
	struct outname	*name;
{
	ind_t		savindex;
	struct outname	*new;

	if ((savindex = savechar(ALLOLCHR, (ind_t)name->on_foff)) == BADOFF)
		return;

	new = (struct outname *)
			core_alloc(ALLOLOCL, (long)sizeof(struct outname));
	if (new != (struct outname *)0) {
		*new = *name;
		new->on_foff = savindex;
	}
}
