#include <system.h>

extern File *out_file;

do_close()
{
	sys_close( out_file);
}
