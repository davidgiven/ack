/* $Id$ */
#include <stdio.h>

FILE *fdopen(fd,mode)
char *mode;
{
	register int i;
	FILE *fp;
	char *malloc();
	int flags = 0;

	if (fd < 0) return NULL;
	for (i = 0; _io_table[i] != 0 ; i++) 
		if ( i >= _NFILES )
			return(NULL);

	switch(*mode) {
	case 'r':
		flags |= IO_READMODE;
		break;
	case 'a':
		lseek(fd, 0L, 2);
	case 'w':
		flags |= IO_WRITEMODE;
		break;
	default:
		return NULL;
	}

	if (( fp = (FILE *) malloc (sizeof( FILE))) == NULL ) {
		return(NULL);
	}

	fp->_count = 0;
	fp->_fd = fd;
	fp->_flags = flags;
	fp->_buf = 0;
	_io_table[i] = fp;
	return(fp);
}
