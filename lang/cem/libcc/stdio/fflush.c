/* $Id$ */
#include <stdio.h>


fflush(iop)
FILE *iop;
{
	int count, c1;

	if (!iop->_buf ||
	    io_testflag(iop,IO_UNBUFF) ||
	    !io_testflag(iop,IO_WRITEMODE) ) 
		return(0);

	count = iop->_bufsiz - iop->_count;
	if ( count <= 0 )
		return(0);

	c1 = write(iop->_fd,iop->_buf,count);

	if ( count == c1 ) {
		iop->_count = iop->_bufsiz;
		iop->_ptr   = iop->_buf;
		return(count);
	}

	iop->_flags |= IO_ERR;
	return(EOF); 
}
