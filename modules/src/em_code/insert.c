/* $Header$ */

/* Implementation of C_insertpart, C_beginpart, and C_endpart.
   Basic methodology: place the parts either in memory or on a temporary
   file, in the order received, and remember this order. Then, in a second
   "pass", write out the code.
   An optimization is possible: as long as the order in which the parts
   are received corresponds to the order in which they must be written,
   they can be written immediately.
*/

#include <alloc.h>
#include "insert.h"

#ifdef INCORE
#define		C_switchtotmp()		(C_ontmpfile = 1)
#define		C_switchtoout()		(C_ontmpfile = 0)
#endif

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

		pp->pp_end = C_current_out;
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

	C_switchtotmp();
	C_curr_part = p;
	pp->pp_next = p->p_parts;
	p->p_parts = pp;
	pp->pp_type = TEXT;
	pp->pp_begin = C_current_out;
}

C_insertpart(part)
	int part;
{
	/*	Insert part "part" in the current part. If C_sequential is
		still set and the part to be inserted is available now,
		just write it out.
	*/
	register Part *p;
	register PartOfPart *pp;

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

C_beginpart(part)
	int part;
{
	/*	Now follows the definition for part "part".
		Suspend the current part, and add part "part" to the
		table.
	*/
	register Part *p = mkpart(part);

	end_partofpart(C_curr_part);

	p->p_prevpart = C_curr_part;
	resume(p);
}

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
		C_switchtoout();
	}
}
