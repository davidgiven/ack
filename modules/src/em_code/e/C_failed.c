#include <system.h>

C_failed()
{
	sys_write(2,"write failed\n",13);
	sys_stop(S_EXIT);
}
