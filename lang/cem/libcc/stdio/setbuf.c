/* $Id$ */
#include	<stdio.h>

setbuf(iop, buffer)
register FILE *iop;
char *buffer;
{
	if ( iop->_buf && io_testflag(iop,IO_MYBUF) )
		free(iop->_buf);

	iop->_flags &= ~(IO_MYBUF | IO_UNBUFF | IO_PERPRINTF);

	iop->_buf = (unsigned char *) buffer;

	iop->_count = 0;
	if ( iop->_buf == NULL ) {
		iop->_flags |= IO_UNBUFF;
		iop->_bufsiz = 1;
	} else {
		if (io_testflag(iop, IO_WRITEMODE)) iop->_count = BUFSIZ;
		iop->_bufsiz = BUFSIZ;
	}
	iop->_ptr = iop->_buf;
}
