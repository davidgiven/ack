#include	<system.h>

No_Mem()
{
	(void) sys_write(2, "Out of memory\n", 14);
	sys_stop(S_EXIT);
}
