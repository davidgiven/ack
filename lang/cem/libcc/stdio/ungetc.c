#include <stdio.h>

ungetc(ch, iop)
int ch;
FILE *iop;
{
	if ( ch < 0  || !io_testflag(iop,IO_READMODE))
		return EOF;
	if (iop->_ptr == iop->_buf) {
		if (iop->_count != 0) return EOF;
		iop->_ptr++;
	}
	iop->_count++;
	*--iop->_ptr = ch;
	return(ch);
}
