#include	<pc_file.h>

notext(f) struct file *f; {
	f->flags &= ~TXTBIT;
}
