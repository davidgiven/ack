#include	<pc_file.h>
#include	<pc_err.h>

extern struct file	*_curfil;
extern			_trp();

_wf(f) struct file *f; {

	_curfil = f;
	if ((f->flags&0377) != MAGIC)
		_trp(EBADF);
	if ((f->flags & WRBIT) == 0)
		_trp(EWRITEF);
}
