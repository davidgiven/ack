#include	<pc_file.h>

extern		_rf();
extern		_incpt();

int _rdc(f) struct file *f; {
	int c;

	_rf(f);
	c = *f->ptr;
	_incpt(f);
	return(c);
}
