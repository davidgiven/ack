/* $Id$ */
#include <stdio.h>

#define  PMODE    0666


FILE *fopen(name,mode)
char *name , *mode;
{
	register int i;
	FILE *fp;
	char *malloc();
	int fd,
	flags = 0;

	for (i = 0; _io_table[i] != 0 ; i++) 
		if ( i >= _NFILES )
			return(NULL);

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
		if (( fd = open (name, 0)) < 0 )
			return(NULL);
		break;

	default:
		return(NULL);
	}


	if (fd < 0) return NULL;

	if (( fp = (FILE *) malloc (sizeof( FILE))) == NULL ) {
		close(fd);
		return(NULL);
	}

	fp->_count = 0;
	fp->_fd = fd;
	fp->_flags = flags;
	fp->_buf = 0;
	_io_table[i] = fp;
	return(fp);
}
