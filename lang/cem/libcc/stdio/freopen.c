/* $Header$ */
#include <stdio.h>

#define  PMODE    0666


FILE *freopen(name,mode,fp)
char *name , *mode;
FILE *fp;
{
	char *malloc();
	int fd,
	flags = 0;

	fflush(fp);
	close(fileno(fp));
	if (io_testflag(fp, IO_MYBUF) && fp->_buf) free(fp->_buf);

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
		free(fp);
		return NULL;
	}

	fp->_count = 0;
	fp->_fd = fd;
	fp->_flags = flags;
	fp->_buf = 0;
	return(fp);
}
