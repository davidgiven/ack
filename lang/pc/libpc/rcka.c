/* Author: Hans van Eck */

#include	<em_abs.h>

extern trp();

struct array_descr	{
		int		lbound;
		unsigned	n_elts_min_one;
		unsigned	size;		/* doesn't really matter */
	    };

_rcka(descr, index)
struct array_descr *descr;
{
	if( index < descr->lbound ||
	    index > (int) descr->n_elts_min_one + descr->lbound )
		_trp(ERANGE);
}
