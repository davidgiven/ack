/*
 * stdio.h - input/output definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(_STDIO_H)
#define	_STDIO_H

/*
 * Focus point of all stdio activity.
 */
typedef struct __iobuf {
	int		_count;
	int		_fd;
	int		_flags;
	int		_bufsiz;
	unsigned char	*_buf;
	unsigned char	*_ptr;
} FILE;

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

/* The following definitions are also in <unistd.h>. They should not
 * conflict.
 */
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

#define	stdin		(&__stdin)
#define	stdout		(&__stdout)
#define	stderr		(&__stderr)

#define	BUFSIZ		1024
#define	NULL		((void *)0)
#define	EOF		(-1)

#define	FOPEN_MAX	20

#if	defined(__BSD4_2)
#define	FILENAME_MAX	255
#else
#define	FILENAME_MAX	14
#endif	/* __BSD4_2 */
#define	TMP_MAX		999
#define	L_tmpnam	(sizeof("/tmp/") + 15)

typedef long int	fpos_t;

#if	!defined(_SIZE_T)
#define	_SIZE_T
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _SIZE_T */

extern FILE	*__iotab[FOPEN_MAX];
extern FILE	__stdin, __stdout, __stderr;

int	remove(const char *_filename);
int	rename(const char *_old, const char *_new);
FILE	*tmpfile(void);
char	*tmpnam(char *_s);
int	fclose(FILE *_stream);
int	fflush(FILE *_stream);
FILE	*fopen(const char *_filename, const char *_mode);
FILE	*freopen(const char *_filename, const char *_mode, FILE *_stream);
void	setbuf(FILE *_stream, char *_buf);
int	setvbuf(FILE *_stream, char *_buf, int _mode, size_t _size);
int	fprintf(FILE *_stream, const char *_format, ...);
int	fscanf(FILE *_stream, const char *_format, ...);
int	printf(const char *_format, ...);
int	scanf(const char *_format, ...);
int	sprintf(char *_s, const char *_format, ...);
int	sscanf(const char *_s, const char *_format, ...);
int	vfprintf(FILE *_stream, const char *_format, char *_arg);
int	vprintf(const char *_format, char *_arg);
int	vsprintf(char *_s, const char *_format, char *_arg);
int	fgetc(FILE *_stream);
char	*fgets(char *_s, int _n, FILE *_stream);
int	fputc(int _c, FILE *_stream);
int	fputs(const char *_s, FILE *_stream);
int	getc(FILE *_stream);
int	getchar(void);
char	*gets(char *_s);
int	putc(int _c, FILE *_stream);
int	putchar(int _c);
int	puts(const char *_s);
int	ungetc(int _c, FILE *_stream);
size_t	fread(void *_ptr, size_t _size, size_t _nmemb, FILE *_stream);
size_t	fwrite(const void *_ptr, size_t _size, size_t _nmemb, FILE *_stream);
int	fgetpos(FILE *_stream, fpos_t *_pos);
int	fseek(FILE *_stream, long _offset, int _whence);
int	fsetpos(FILE *_stream, fpos_t *_pos);
long	ftell(FILE *_stream);
void	rewind(FILE *_stream);
void	clearerr(FILE *_stream);
int	feof(FILE *_stream);
int	ferror(FILE *_stream);
void	perror(const char *_s);

int __fillbuf(FILE *_stream);
int __flushbuf(int _c, FILE *_stream);


#define	getchar()	getc(stdin)
#define	putchar(c)	putc(c,stdout)
#define	getc(p)		(--(p)->_count >= 0 ? (int) (*(p)->_ptr++) : \
				__fillbuf(p))
#define	putc(c, p)	(--(p)->_count >= 0 ? \
			 (int) (*(p)->_ptr++ = (c)) : \
			 __flushbuf((c),(p)))

#define	feof(p)		(((p)->_flags & _IOEOF) != 0)
#define	ferror(p)	(((p)->_flags & _IOERR) != 0)
#define clearerr(p)	((p)->_flags &= ~(_IOERR|_IOEOF))

#if	defined(__BSD4_2) || defined(__USG) || defined(_POSIX_SOURCE)
int fileno(FILE *_stream);
FILE *fdopen(int fildes, const char *type);
#define	fileno(stream)		((stream)->_fd)
#endif	/* __BSD4_2 || __USG || _POSIX_SOURCE */

#endif	/* _STDIO_H */
