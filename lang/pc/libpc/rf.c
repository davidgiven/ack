#include	<pc_file.h>
#include	<pc_err.h>

extern struct file	*_curfil;
extern			_trp();
extern			_incpt();

_rf(f) struct file *f; {

	_curfil = f;
	if ((f->flags&0377) != MAGIC)
		_trp(EBADF);
	if (f->flags & WRBIT)
		_trp(EREADF);
	if ((f->flags & WINDOW) == 0)
		_incpt(f);
}
