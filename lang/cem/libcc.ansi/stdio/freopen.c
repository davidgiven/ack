/*
 * freopen.c - open a file and associate a stream with it
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdlib.h>
#include	"loc_incl.h"

#define	PMODE		0666

/* Do not "optimize" this file to use the open with O_CREAT if the file
 * does not exist. The reason is given in fopen.c.
 */
#define	O_RDONLY	0
#define	O_WRONLY	1
#define	O_RDWR		2

#define	O_CREAT		0x010
#define	O_TRUNC		0x020
#define	O_APPEND	0x040

int _open(const char *path, int flags);
int _creat(const char *path, int mode);
int _close(int d);

FILE *
freopen(const char *name, const char *mode, FILE *stream)
{
	register int i;
	int rwmode = 0, rwflags = 0;
	int fd, flags = stream->_flags & (_IONBF | _IOFBF | _IOLBF | _IOMYBUF);

	(void) fflush(stream);				/* ignore errors */
	(void) _close(fileno(stream));

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
		flags |= _IOWRITE | _IOAPPEND;
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

	if ((rwflags & O_TRUNC)
	    || (((fd = _open(name, rwmode)) < 0)
		    && (flags & _IOWRITE)))
		fd = _creat(name, PMODE);

	if (fd < 0) {
		for( i = 0; i < FOPEN_MAX; i++) {
			if (stream == __iotab[i]) {
				__iotab[i] = 0;
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
