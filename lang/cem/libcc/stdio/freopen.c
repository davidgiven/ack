/* $Id$ */
#include <stdio.h>

#define  PMODE    0666


FILE *freopen(name,mode,fp)
char *name , *mode;
register FILE *fp;
{
	int fd,
	flags = fp->_flags & ~(IO_WRITEMODE|IO_READMODE|IO_ERR|IO_EOF|IO_PERPRINTF);

	fflush(fp);
	close(fileno(fp));

	switch(*mode){

	case 'w':
		flags |= IO_WRITEMODE;
		fd = creat (name,PMODE);
		break;

	case 'a': 
		flags |= IO_WRITEMODE;
		if (( fd = open(name, 1)) < 0 )
			fd = creat(name, PMODE);
		if (fd >= 0) lseek(fd,0L,2);
		break;         

	case 'r':
		flags |= IO_READMODE;	
		fd = open(name, 0);
		break;

	default:
		fd = -1;
	}

	if (fd < 0) {
		register int i;

		for (i = 0; i < _NFILES; i++) {
			if (fp == _io_table[i]) {
				_io_table[i] = 0;
				break;
			}
		}
		if (fp != &_stdin && fp != &_stdout && fp != &_stderr) free(fp);
		return NULL;
	}
	fp->_count = 0;
	if (fp->_buf && !(flags & IO_UNBUFF) && (flags & IO_WRITEMODE)) 
		fp->_count = fp->_bufsiz;
	fp->_fd = fd;
	fp->_flags = flags;
	return(fp);
}
