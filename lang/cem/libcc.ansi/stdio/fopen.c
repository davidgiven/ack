/*
 * fopen.c - open a stream
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/file.h>
#include	"loc_incl.h"

#define	PMODE		0666

int open(const char *path, int flags, int mode);
int close(int d);

FILE *
fopen(const char *name, const char *mode)
{
	register int i;
	int rwmode = 0, rwflags = 0;
	FILE *stream;
	int fd, flags = 0;

	for (i = 0; _iotable[i] != 0 ; i++) 
		if ( i >= FOPEN_MAX )
			return (FILE *)NULL;

	switch(*mode++) {

	case 'r':
		flags |= _IOREAD;	
		rwmode = O_RDONLY;
		break;

	case 'w':
		flags |= _IOWRITE;
		rwmode = O_WRONLY;
		rwflags = O_CREAT | O_TRUNC;
		break;

	case 'a': 
		flags |= _IOWRITE;
		rwmode = O_WRONLY;
		rwflags |= O_APPEND | O_CREAT;
		break;         

	default:
		return (FILE *)NULL;
	}

	while (*mode) {
		switch(*mode++) {

		case 'b':
			break;

		case '+':
			rwmode = O_RDWR;
			flags |= _IOREAD | _IOWRITE;
			break;

		default:
			return (FILE *)NULL;

		}
	}

	fd = open(name, rwmode | rwflags, PMODE);

	if (fd < 0) return (FILE *)NULL;

	if (( stream = (FILE *) malloc(sizeof(FILE))) == NULL ) {
		close(fd);
		return (FILE *)NULL;
	}

	stream->_count = 0;
	stream->_fd = fd;
	stream->_flags = flags;
	stream->_buf = 0;
	_iotable[i] = stream;
	return stream;
}
