/* $Header$ */

/* Implementation of C_insertpart, C_beginpart, and C_endpart.
   Basic methodology: place the parts either in memory or on a temporary
   file, in the order received, and remember this order. Then, in a second
   "pass", write out the code.
   An optimization is possible: as long as the order in which the parts
   are received corresponds to the order in which they must be written,
   they can be written immediately.
*/

#include <em_path.h>
#include <alloc.h>
#include "insert.h"

char		*C_tmpdir = TMP_DIR;
static Part	*C_stable[TABSIZ];

static char	*C_old_top;
static char	*C_old_opp;

#ifndef INCORE
static File	*C_old_ofp;

static int
getbyte(b)
	long b;
{
	/*	Get the byte at offset "b" from the start of the
		temporary file, and try to do so in an efficient way.
	*/
	static long	start_core, curr_pos;

	if (b < start_core || b >= curr_pos) {
		/* the block wanted is not in core, so get it */
		long nb = (b & ~(BUFSIZ - 1));
		int n;

		C_flush();
		if (nb != curr_pos) {
			if (sys_seek(C_tfr, nb, 0, &curr_pos) == 0) {
				C_failed();
			}
		}
		if (! C_ibuf) {
			C_ibuf = Malloc(BUFSIZ);
		}
		if (sys_read(C_tfr, C_ibuf, BUFSIZ, &n) == 0) {
			C_failed();
		}
		curr_pos += n;
		start_core = nb;
	}

	return C_ibuf[(int) (b - start_core)];
}
#endif

static C_out_parts();
static Part *C_findpart();

static
outpart(id)
	int id;
{
	/*	Output part "id", if present.
	*/
	Part *p = C_findpart(id);

	if (p) {
		C_out_parts(p->p_parts);
		p->p_parts = 0;
	}
}

static
C_out_parts(pp)
	register PartOfPart *pp;
{
	/*	Output the list of chunks started by "pp".
		The list is build in reverse order, so this routine is
		recursive.
	*/
	PartOfPart *prev = 0, *next;

	while (pp) {
		next = pp->pp_next;
		pp->pp_next = prev;
		prev = pp;
		pp = next;
	}
	pp = prev;

	while (pp) {
		if (pp->pp_type == INSERT) {
			(*C_outpart)(pp->pp_id);
		}
		else {
			/* copy the chunk to output */
#ifdef INCORE
			register char *s = C_BASE + pp->pp_begin;
			char *se = C_BASE + pp->pp_end;

			while (s < se) {
				put(*s++);
			}
#else
			register long b = pp->pp_begin;

			while (b < pp->pp_end) {
				put(getbyte(b++));
			}
#endif
		}
		prev = pp;
		pp = pp->pp_next;
		free((char *) prev);
	}
}

static Part *
C_findpart(part)
	int part;
{
	/*	Look for part "part" in the table.
		Return 0 if not present,
	*/
	register Part *p = C_stable[part % TABSIZ];

	while (p && p->p_id != part) {
		p = p->p_next;
	}
	return p;
}

extern char	*strcpy(), *strcat(), *mktemp();

static
swttmp()
{
#ifndef INCORE
	if (C_tmpfile == 0) {
		static char tmpbuf[64];
		register char *p = tmpbuf;

		strcpy(p, C_tmpdir);
		strcat(p, "/CodeXXXXXX");
		C_tmpfile = mktemp(p);
		if (! sys_open(p, OP_WRITE, &C_old_ofp)) {
			C_failed();
		}
		if (! sys_open(p, OP_READ, &C_tfr)) {
			C_failed();
		}
	}
	if (! C_ontmpfile) {
		File *p = C_ofp;

		C_flush();
		C_ofp = C_old_ofp;
		C_old_ofp = p;
		C_ontmpfile = 1;
	}
#else
	if (! C_ontmpfile) {
		char *p;

		p = C_opp;
		C_opp = C_old_opp;
		C_old_opp = p;

		p = C_top;
		C_top = C_old_top;
		C_old_top = p;
		C_ontmpfile = 1;
	}
#endif
}

static
swtout()
{
#ifndef INCORE
	if (C_ontmpfile) {
		File *p = C_ofp;

		C_flush();
		C_ofp = C_old_ofp;
		C_old_ofp = p;
		C_ontmpfile = 0;
	}
#else
	if (C_ontmpfile) {
		char *p;

		p = C_opp;
		C_opp = C_old_opp;
		C_old_opp = p;

		p = C_top;
		C_top = C_old_top;
		C_old_top = p;
		C_ontmpfile = 0;
	}
#endif
}

static int
available(part)
	int part;
{
	/*	See if part "part", and all the parts it consists of,
		are available. Return 1 if they are, 0 otherwize
	*/
	register Part *p = C_findpart(part);
	register PartOfPart *pp;
	int retval = 1;

	if (p == 0) return 0;

	if (p->p_flags & BUSY) {
		/* recursive call ends up here, and this just should
		   not happen. It is an error of the programmer using
		   this module.
		*/
		C_internal_error();
	}

	p->p_flags |= BUSY;

	pp = p->p_parts;
	while (pp) {
		if (pp->pp_type == INSERT && ! available(pp->pp_id)) {
			retval = 0;
			break;
		}
		else	pp = pp->pp_next;
	}
	p->p_flags &= ~BUSY;
	return retval;
}

static Part *
mkpart(part)
	int part;
{
	/*	Create a Part structure with id "part", and return a
		pointer to it, after checking that is does not exist
		already.
	*/
	register Part *p = C_findpart(part);
	register int index = part % TABSIZ;
	
	if (p != 0) {
		/* multiple defined part ... */
		C_internal_error();
	}

	p = (Part *) Malloc(sizeof(Part));
	p->p_id = part;
	p->p_next = C_stable[index];
	C_stable[index] = p;
	p->p_parts = 0;
	p->p_flags = 0;
	p->p_prevpart = 0;
	return p;
}

static
end_partofpart(p)
	register Part *p;
{
	/*	End the current chunk of part *p.
	*/

	if (p) {
		register PartOfPart *pp = p->p_parts;

		pp->pp_end = C_current_out - C_BASE;
		if (pp->pp_begin == pp->pp_end) {
			/* nothing in this chunk, so give it back */
			p->p_parts = pp->pp_next;
			free((char *) pp);
		}
	}
}

static
resume(p)
	register Part *p;
{
	/*	Resume part "p", by creating a new PartOfPart structure
		for it.
	*/
	register PartOfPart *pp = (PartOfPart *) Malloc(sizeof(PartOfPart));

	swttmp();
	C_curr_part = p;
	pp->pp_next = p->p_parts;
	p->p_parts = pp;
	pp->pp_type = TEXT;
	pp->pp_begin = C_current_out - C_BASE;
}

void
C_insertpart(part)
	int part;
{
	/*	Insert part "part" in the current part. If C_sequential is
		still set and the part to be inserted is available now,
		just write it out.
	*/
	register Part *p;
	register PartOfPart *pp;

	C_outpart = outpart;
	C_swttmp = swttmp;
	C_swtout = swtout;
	if (C_sequential && available(part)) {
		outpart(part);
		return;
	}

	if (C_sequential) {
		/* stop the sequential stuff, by creating a part */
		C_sequential = 0;
		p = mkpart(0);
		C_curr_part = p;
	}
	else {
		p = C_curr_part;
		end_partofpart(p);
	}

	/* Now, add the insertion of "part" to the current part.  */
	pp = (PartOfPart *) Malloc(sizeof(PartOfPart));
	pp->pp_next = p->p_parts;
	p->p_parts = pp;
	pp->pp_type = INSERT;
	pp->pp_id = part;
	resume(p);
}

void
C_beginpart(part)
	int part;
{
	/*	Now follows the definition for part "part".
		Suspend the current part, and add part "part" to the
		table.
	*/
	register Part *p = mkpart(part);

	C_outpart = outpart;
	C_swttmp = swttmp;
	C_swtout = swtout;

	end_partofpart(C_curr_part);

	p->p_prevpart = C_curr_part;
	resume(p);
}

void
C_endpart(part)
	int part;
{
	/*	End the current part. The parameter "part" is just there
		for the checking. Do we really need it ???
	*/
	register Part *p = C_curr_part;

	if (p->p_id != part) {
		/* illegal C_endpart ... */
		C_internal_error();
	}

	end_partofpart(p);
	if (p->p_prevpart) resume(p->p_prevpart);
	else {
		C_curr_part = 0;
		swtout();
	}
}
