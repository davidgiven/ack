/* $Id$ */
#include <stdio.h>

fclose(fp)
FILE *fp;
{
	register int i;

	for (i=0; i<_NFILES; i++)
		if (fp == _io_table[i]) {
			_io_table[i] = 0;
			break;
		}
	if (i >= _NFILES)
		return(EOF);
	fflush(fp);
	close(fileno(fp));
	if ( io_testflag(fp,IO_MYBUF) && fp->_buf )
		free( fp->_buf );
	if (fp != &_stdin && fp != &_stdout && fp != &_stderr) free(fp);
	return(NULL);
}

