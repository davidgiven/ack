/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define BUFSIZ  1024
#ifdef __vax
#define _NBF	8
#endif
#ifdef __mc68020
#define _NBF	8
#endif
#ifndef _NBF
#define _NBF	1
#endif
#define _BUFSIZ	(_NBF * BUFSIZ)
#define _NFILES  20
#define NULL       0
#define EOF     (-1)

#define IO_READMODE     1
#define IO_WRITEMODE    2
#define IO_UNBUFF       4
#define IO_EOF         8
#define IO_ERR        16
#define IO_MYBUF     32
#define IO_PERPRINTF   64

#ifndef FILE

extern struct _io_buf {
    int     _count;
    int     _flags;
    unsigned char   *_buf;
    unsigned char   *_ptr;
    int	    _bufsiz;
    int     _fd;
}  *_io_table[_NFILES], _stdin, _stdout, _stderr;


#endif	/* FILE */

#define FILE struct _io_buf


#define stdin  (&_stdin)
#define stdout 	(&_stdout)
#define stderr 	(&_stderr)

#define getchar() 		getc(stdin)
#define putchar(c) 		putc(c,stdout)
#define getc(p)			(--(p)->_count >= 0 ? (int) (*(p)->_ptr++) : \
					_fillbuf(p))
#define putc(c, p)		(--(p)->_count >= 0 ? \
				 (int) (*(p)->_ptr++ = (c)) : \
				 _flushbuf((c),(p)))
#define feof(p) 		(((p)->_flags & IO_EOF) != 0)
#define ferror(p) 		(((p)->_flags & IO_ERR) != 0)
#define fileno(p) 		((p)->_fd)
#define io_testflag(p,x)	((p)->_flags & (x))

/* If you want a stream to be flushed after each printf use:
 * 
 *	io_perprintf(stream);
 *
 * If you want to stop with this kind of buffering use:
 *
 *	io_noperprintf(stream);
 */

#define io_noperprintf(p)	((p)->_flags &= ~IO_PERPRINTF)
#define io_perprintf(p)		((p)->_flags |= IO_PERPRINTF)

extern FILE *fopen(), *fdopen(), *freopen(), *popen();
extern long ftell();
extern setbuf(), rewind();
extern char *fgets(), *gets();
