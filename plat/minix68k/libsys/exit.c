#include "lib.h"
#include <unistd.h>

PUBLIC int exit(int status)
{
	_cleanup();
	_exit(status);
}
