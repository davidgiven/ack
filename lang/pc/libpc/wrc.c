#include	<pc_file.h>

extern		_wf();
extern		_outcpt();

_wrc(c,f) int c; struct file *f; {
	*f->ptr = c;
	_wf(f);
	_outcpt(f);
}

_wln(f) struct file *f; {
#ifdef CPM
	_wrc('\r',f);
#endif
	_wrc('\n',f);
	f->flags |= ELNBIT;
}

_pag(f) struct file *f; {
	_wrc('\014',f);
	f->flags |= ELNBIT;
}
