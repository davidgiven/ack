/* $Header$ */
#include	<pc_file.h>

extern		_cls();

/* procedure pclose(var f:file of ??); */

pclose(f) struct file *f; {
	_cls(f);
}
