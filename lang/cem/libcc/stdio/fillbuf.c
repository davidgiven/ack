/* $Id$ */
#include <stdio.h>

char *malloc();

_fillbuf(iop)
register FILE *iop;
{
	static unsigned char ch[_NFILES];

	iop->_count = 0;
	if (fileno(iop) < 0) return EOF;
	if ( io_testflag(iop, (IO_EOF | IO_ERR )))
		return (EOF); 

	if ( !io_testflag(iop, IO_READMODE) ) 
		return (EOF);

	if (! io_testflag(iop, IO_UNBUFF) && ! iop->_buf) {
		iop->_buf = (unsigned char *) malloc(_BUFSIZ);
		if (! iop->_buf) {
			iop->_flags |= IO_UNBUFF;
		}
		else {
			iop->_flags |= IO_MYBUF;
			iop->_bufsiz = _BUFSIZ;
		}
	}
	if (! iop->_buf) {
		iop->_buf = &ch[fileno(iop)];
		iop->_bufsiz = 1;
	}
	iop->_ptr = iop->_buf;
	iop->_count = read(iop->_fd, iop->_buf, iop->_bufsiz);

	if (iop->_count <= 0){
		if (iop->_count == 0) {
			iop->_flags |= IO_EOF;
		}
		else 
			iop->_flags |= IO_ERR;

		return (EOF);
	}
	iop->_count--;

	return *iop->_ptr++;
}
