#ifndef ACK_EMUFILE_H
#define ACK_EMUFILE_H

/*
 * Focus point of all stdio activity.
 */
struct FILE {
	int		_count;
	int		_fd;
	int		_flags;
	int		_bufsiz;
	unsigned char	*_buf;
	unsigned char	*_ptr;
};

#define	_IOFBF		0x000
#define	_IOREAD		0x001
#define	_IOWRITE	0x002
#define	_IONBF		0x004
#define	_IOMYBUF	0x008
#define	_IOEOF		0x010
#define	_IOERR		0x020
#define	_IOLBF		0x040
#define	_IOREADING	0x080
#define _IOWRITING	0x100
#define	_IOAPPEND	0x200

#if !defined BUFSIZ
#define	BUFSIZ      1024
#endif

#define	FOPEN_MAX	20
extern FILE	*__iotab[FOPEN_MAX];

#define	stdin       (&__stdin)
#define	stdout      (&__stdout)
#define	stderr      (&__stderr)
extern FILE	__stdin, __stdout, __stderr;

#define	FILENAME_MAX 255
#define	TMP_MAX     999
#define	L_tmpnam    (sizeof("/tmp/") + 15)
#define ACK_TMP_PREFIX "/tmp/tmp."

extern int __fillbuf(FILE *_stream);
extern int __flushbuf(int _c, FILE *_stream);

#define	getc(p)		(--(p)->_count >= 0 ? (int) (*(p)->_ptr++) : \
				__fillbuf(p))
#define	putc(c, p)	(--(p)->_count >= 0 ? \
			 (int) (*(p)->_ptr++ = (c)) : \
			 __flushbuf((c),(p)))

#define	feof(p)		(((p)->_flags & _IOEOF) != 0)
#define	ferror(p)	(((p)->_flags & _IOERR) != 0)
#define clearerr(p)	((p)->_flags &= ~(_IOERR|_IOEOF))

/* Non-standard extensions */

extern int fileno(FILE *_stream);
extern FILE* fdopen(int fildes, const char *type);
#define	fileno(stream) ((stream)->_fd)

#define	io_testflag(p,x)	((p)->_flags & (x))
extern void __register_stdio_cleanup(void);

#endif
