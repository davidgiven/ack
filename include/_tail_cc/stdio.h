#define BUFSIZ  1024
#define _NFILES  20
#define NULL       0
#define EOF     (-1)
#define CMASK   0377

#define IO_READMODE     1
#define IO_WRITEMODE    2
#define IO_UNBUFF       4
#define IO_EOF         8
#define IO_ERR        16
#define IO_MYBUF     32
#define IO_PERPRINTF   64

#ifndef FILE

extern struct _io_buf {
    int     _fd;
    int     _count;
    int     _flags;
    unsigned char   *_buf;
    unsigned char   *_ptr;
}  *_io_table[_NFILES];


#endif	/* FILE */

#define FILE struct _io_buf


#define stdin  (_io_table[0])	
#define stdout 	(_io_table[1])
#define stderr 	(_io_table[2])

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
