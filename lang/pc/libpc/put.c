/* $Header$ */
#include	<pc_file.h>

extern		_wf();
extern		_outcpt();

_put(f) struct file *f; {
	_wf(f);
	_outcpt(f);
}
