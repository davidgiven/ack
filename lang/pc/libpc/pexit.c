#include	<pc_file.h>

extern int	*_extfl;
extern char	*_hbase;
extern		_wrs();
extern		_wln();

procexit(name) char *name; {
	struct file *f;

	f = EXTFL(2);
	_wrs(5,"exit ",f);
	_wrs(8,name,f);
	_wln(f);
}
