/* $Id$ */
#include <stdio.h>


fseek(iop, offset, where)
FILE *iop;
long offset;
{
	int  count;
	long lseek();
	long pos;

	iop->_flags &= ~(IO_EOF | IO_ERR);
	/* Clear both the end of file and error flags */

	if ( io_testflag(iop,IO_READMODE) ) {
		if ( where < 2 && iop->_buf && !io_testflag(iop,IO_UNBUFF) ) {
			count = iop->_count;
			pos = offset;

			if ( where == 0 )
				pos += count - lseek(fileno(iop), 0L,1);
			else
				offset -= count;

			if ( count > 0 && pos <= count 
			     && pos >= iop->_buf - iop->_ptr ) {
		        	iop->_ptr += (int) pos;
				iop->_count -= (int) pos;
				return(0);
			}
		}
		pos = lseek(fileno(iop), offset, where);
		iop->_count = 0;
	} else if ( io_testflag(iop,IO_WRITEMODE) ) {
		fflush(iop);
		pos = lseek(fileno(iop), offset, where);
	}
	return((pos == -1) ? -1 : 0 );
}
