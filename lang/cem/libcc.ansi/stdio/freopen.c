/*
 * freopen.c - open a file and associate a stream with it
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/file.h>
#include	"loc_incl.h"

#define  PMODE    0666
int open(const char *path, int flags, int mode);
int close(int d);

FILE *
freopen(const char *name, const char *mode, FILE *stream)
{
	register int i;
	int rwmode = 0, rwflags = 0;
	int fd, flags = stream->_flags & ~(_IOWRITE|_IOREAD|_IOERR|_IOEOF);

	(void) fflush(stream);				/* ignore errors */
	(void) close(fileno(stream));

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

	if (fd < 0) {
		for( i = 0; i < FOPEN_MAX; i++) {
			if (stream == _iotable[i]) {
				_iotable[i] = 0;
				break;
			}
		}
		if (stream != stdin && stream != stdout && stream != stderr)
			free((void *)stream);
		return (FILE *)NULL;
	}

	stream->_count = 0;
	if (stream->_buf && !(flags & _IONBF) && (flags & _IOWRITE)) 
		if (flags & _IOLBF)
			stream->_count  = 0;
		else	stream->_count = stream->_bufsiz;

	stream->_fd = fd;
	stream->_flags = flags;
	return stream;
}
