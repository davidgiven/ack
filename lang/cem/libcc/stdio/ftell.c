/* $Header$ */
#include <stdio.h>


long ftell(iop)
FILE *iop;
{
	long result;
	long lseek();
	int adjust = 0;

	if ( io_testflag(iop,IO_READMODE) )
		adjust -= iop->_count;
	else if ( io_testflag(iop,IO_WRITEMODE) && iop->_buf && !io_testflag(iop,IO_UNBUFF))
		adjust = iop->_ptr - iop->_buf;
	
	result = lseek(fileno(iop), 0L, 1);

	if ( result < 0 )
		return ( result );

	result += (long) adjust;
	return(result);
}
