/* $Header$ */
#include	<pc_file.h>

extern struct file	*_curfil;
extern			_incpt();

char *_wdw(f) struct file *f; {

	_curfil = f;
	if ((f->flags & (WINDOW|WRBIT|0377)) == MAGIC)
		_incpt(f);
	return(f->ptr);
}
