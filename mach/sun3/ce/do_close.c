#include <system.h>

extern File *_out_file;

close_back()
{
	sys_close( _out_file);
}
