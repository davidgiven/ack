#include	<pc_file.h>
#include	<pc_err.h>

extern		_rf();
extern		_trp();

_get(f) struct file *f; {

	_rf(f);
	if (f->flags&EOFBIT)
		_trp(EEOF);
	f->flags &= ~WINDOW;
}
