#include	<stdio.h>


setbuf(iop, buffer)
register FILE *iop;
char *buffer;
{
	if ( iop->_buf && io_testflag(iop,IO_MYBUF) )
		free(iop->_buf);

	iop->_flags &= ~(IO_MYBUF | IO_UNBUFF | IO_PERPRINTF);

	iop->_buf = (unsigned char *) buffer;

	if ( iop->_buf == NULL )
		iop->_flags |= IO_UNBUFF;

	iop->_ptr = iop->_buf;
	iop->_count = 0;
}
