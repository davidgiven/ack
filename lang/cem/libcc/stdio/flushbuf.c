#include <stdio.h>

int
_flushbuf(c, iop)
	register FILE *iop;
{
	if (fileno(iop) < 0) return EOF;
	if (! io_testflag(iop, IO_UNBUFF)) {
		if (iop->_buf == 0) {
			if (iop == stdout) {
				if (isatty(fileno(stdout))) {
					iop->_flags |= IO_UNBUFF;
				}
				else {
					extern unsigned char _sobuf[];

					iop->_buf = iop->_ptr = _sobuf;
					iop->_count = BUFSIZ;
				}
			}
			else {
				extern char *malloc();

				if (!(iop->_buf = (unsigned char *) malloc(BUFSIZ))) {
					iop->_flags |= IO_UNBUFF;
				}
				else {
					iop->_ptr = iop->_buf;
					iop->_flags |= IO_MYBUF;
					iop->_count = BUFSIZ;
				}
			}
		}
	}

	if (io_testflag(iop, IO_UNBUFF)) {
		char c1 = c;

		iop->_count = 0;
		if (write(fileno(iop), &c1, 1) != 1) {
			iop->_flags |= IO_ERR;
			return EOF;
		}
		return c;
	}
	else {
		int count = BUFSIZ - iop->_count;

		iop->_count = BUFSIZ - 1;
		iop->_ptr = iop->_buf + 1;

		if (count > 0) {
			if (write(fileno(iop), iop->_buf, count) != count) {
				*(iop->_buf) = c;
				iop->_flags |= IO_ERR;
				return EOF;
			}
		}
		*(iop->_buf) = c;
	}
	return c;
}

_cleanup()
{
	register int i;

	for ( i = 0 ; i < _NFILES ; i++ )
		if ( _io_table[i] != NULL )
			fclose(_io_table[i]);
}
