/* $Id$ */
#include <stdio.h>

ungetc(ch, iop)
int ch;
register FILE *iop;
{
	unsigned char *p;

	if ( ch < 0  || !io_testflag(iop,IO_READMODE))
		return EOF;
	if (iop->_ptr == iop->_buf) {
		if (iop->_count != 0) return EOF;
		iop->_ptr++;
	}
	iop->_count++;
	p = --(iop->_ptr);	/* ??? Bloody vax assembler !!! */
	*p = ch;
	return(ch);
}
