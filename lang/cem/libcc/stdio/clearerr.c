#include <stdio.h>

clearerr(iop)
	FILE *iop;
{
	iop->_flags &= ~(IO_ERR|IO_EOF);
}
