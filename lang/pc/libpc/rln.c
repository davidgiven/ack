#include	<pc_file.h>

extern		_rf();
extern		_incpt();

_rln(f) struct file *f; {

	_rf(f);
	while ((f->flags & ELNBIT) == 0)
		_incpt(f);
	f->flags &= ~WINDOW;
}
