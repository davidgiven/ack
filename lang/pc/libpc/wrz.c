#include	<pc_file.h>

extern		_wss();
extern		_wrs();

_wsz(w,s,f) int w; char *s; struct file *f; {
	char *p;

	for (p=s; *p; p++);
	_wss(w,p-s,s,f);
}

_wrz(s,f) char *s; struct file *f; {
	char *p;

	for (p=s; *p; p++);
	_wrs(p-s,s,f);
}
