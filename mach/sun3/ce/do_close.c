#include <system.h>
#include <back.h>

extern File *B_out_file;

close_back()
{
	sys_close( B_out_file);
}
