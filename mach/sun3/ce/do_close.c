#include <system.h>

extern File *out_file;

close_back()
{
	sys_close( out_file);
}
