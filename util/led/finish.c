#ifndef lint
static char rcsid[] = "$Header$";
#endif

#include "out.h"
#include "const.h"
#include "defs.h"
#include "memory.h"
#include "orig.h"
#include "scan.h"

extern bool	incore;
extern int	flagword;

/*
 * We know all there is to know about the current module.
 * Now we relocate the values in the emitted bytes and write
 * those to the final output file. Then we compute the relative origins
 * for the next module.
 */
finish()
{
	struct outhead	*head;
	struct outsect	*sects;
	struct outname	*names;
	char		*chars;

	get_modul();
	head = (struct outhead *)modulptr(IND_HEAD);
	sects = (struct outsect *)modulptr(IND_SECT(*head));
	names = (struct outname *)modulptr(IND_NAME(*head));
	chars = (char *)modulptr(IND_CHAR(*head));
	adjust_names(names, head, chars);
	handle_relos(head, sects, names);
	if (!incore && !(flagword & SFLAG)) {
		put_locals(names, head->oh_nname, sects);
#ifdef SYMDBUG
		put_dbug(OFF_DBUG(*head));
#endif SYMDBUG
	}
	compute_origins(sects, head->oh_nsect);
	skip_modul(head);
}

/*
 * Adjust all local names for the move into core.
 */
static
adjust_names(name, head, chars)
	register struct outname	*name;
	struct outhead		*head;
	register char		*chars;
{
	register int		cnt;
	register ind_t		charoff;

	cnt = head->oh_nname;
	charoff = OFF_CHAR(*head);
	while (cnt--) {
		if (name->on_foff != (long)0)
			name->on_mptr = chars + name->on_foff - charoff;
		name++;
	}
}

/*
 * If all sections are in core, we can access them randomly, so we need only
 * scan the relocation table once. Otherwise we must for each section scan
 * the relocation table again, because the relocation entries of one section
 * need not be consecutive.
 */
static
handle_relos(head, sects, names)
	struct outhead		*head;
	struct outsect		*sects;
	struct outname		*names;
{
	register struct outrelo	*relo;
	register int		sectindex;
	register int		nrelo;
	register char		*emit;
	extern char		*getemit();
	extern struct outrelo	*nextrelo();

	if (incore) {
		nrelo = head->oh_nrelo; sectindex = -1;
		startrelo(head); relo = nextrelo();
		while (nrelo--) {
			if (sectindex != relo->or_sect - S_MIN) {
				sectindex = relo->or_sect - S_MIN;
				emit = getemit(head, sects, sectindex);
			}
			relocate(head, emit, names, relo, sects);
			relo++;
		}
	} else {
		for (sectindex = 0; sectindex < head->oh_nsect; sectindex++) {
			emit = getemit(head, sects, sectindex);
			nrelo = head->oh_nrelo; startrelo(head);
			while (nrelo--) {
				relo = nextrelo();
				if (relo->or_sect - S_MIN == sectindex) {
					relocate(head,emit,names,relo,sects);
					/*
					 * Write out the (probably changed)
					 * relocation information.
					 */
					if (flagword & RFLAG)
						wrt_relo(relo);
				}
			}
			wrt_emit(emit, sectindex, sects[sectindex].os_flen);
			/*
			 * XXX We should be able to free the emitted bytes.
			 */
		}
	}
}

/*
 * Write out the local names that must be saved.
 */
static
put_locals(name, nnames, sects)
	register struct outname	*name;
	register ushort		nnames;
	register struct outsect	*sects;
{
	while (nnames--) {
		if ((name->on_type & S_EXT) == 0 && mustsavelocal(name)) {
			namerelocate(name, sects);
			addbase(name);
			wrt_name(name);
		}
		name++;
	}
}

/*
 * Add all flen's and all (size - flen == zero)'s of preceding sections
 * with the same number.
 */
static
compute_origins(sect, nsect)
	register struct outsect	*sect;
	register ushort		nsect;
{
	extern struct orig	relorig[];
	register struct orig	*orig = relorig;

	while (nsect--) {
		orig->org_flen += sect->os_flen;
		orig->org_zero += sect->os_size - sect->os_flen;
		orig++; sect++;
	}
}
#ifdef SYMDBUG

/*
 * Write out what is after the string area. This is likely to be
 * debugging information.
 */
static
put_dbug(offdbug)
	long		offdbug;
{
	char		buf[512];
	register int	nbytes;
	register long	dbugsize;
	extern long	objectsize;
	extern long	position;

	dbugsize = objectsize - offdbug;
	seek(position + offdbug);
	while (dbugsize) {
		nbytes = dbugsize > 512 ? 512 : dbugsize;
		read_char(buf, (long)nbytes);
		wrt_dbug(buf, nbytes);
		dbugsize -= nbytes;
	}
}
#endif SYMDBUG
