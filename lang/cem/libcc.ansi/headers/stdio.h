/*
 * stdio.h - input/output definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	_STDIO_HEADER_
#define	_STDIO_HEADER_

#include	<stdarg.h>

#define	_NFILES		20

/*
 * Focus point of all stdio activity.
 */
typedef struct _iobuf {
	int		_fd;
	int		_count;
	int		_flags;
	char		*_tname;
	unsigned char	*_buf;
	unsigned char	*_ptr;
} FILE;

#define	_IOFBF		0000
#define	_IOREAD		0001
#define	_IOWRITE	0002
#define	_IONBF		0004
#define	_IOMYBUF	0010
#define	_IOEOF		0020
#define	_IOERR		0040
#define	_IOLBF		0100

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

#define	stdin		(&_stdin)
#define	stdout		(&_stdout)
#define	stderr		(&_stderr)

#define	BUFSIZ		1024
#ifndef	NULL
#define	NULL		0
#endif	/* NULL */
#ifndef	EOF
#define	EOF		(-1)
#endif	/* EOF */

#define	FOPEN_MAX	(_NFILES - 3)
#ifdef	__BSD4_2
#define	FILENAME_MAX	255
#else
#define	FILENAME_MAX	14
#endif	/* __BSD4_2 */
#define	TMP_MAX		1000
#define	L_tmpnam	(sizeof("/usr/tmp/") + 15)

#ifndef	_TYPE_FPOS_
#define	_TYPE_FPOS_
typedef long int	fpos_t;
#endif	/* _TYPE_FPOS */

#ifndef	_TYPE_SIZE_
#define	_TYPE_SIZE_
typedef unsigned int	size_t;
#endif	/* _TYPE_SIZE_ */

#define	getchar()	getc(stdin)
#define	putchar(c)	putc(c,stdout)
#define	getc(p)		(--(p)->_count >= 0 ? (int) (*(p)->_ptr++) : \
				_fillbuf(p))
#define	putc(c, p)	(--(p)->_count >= 0 ? \
			 (int) (*(p)->_ptr++ = (c)) : \
			 _flushbuf((c),(p)))

#define	feof(p)		(((p)->_flags & _IOEOF) != 0)
#define	ferror(p)	(((p)->_flags & _IOERR) != 0)
#define	fileno(p)	((p)->_fd)


FILE	*_iotable[_NFILES];
FILE	_stdin, _stdout, _stderr;

#ifdef	__STDC__
int	remove(const char *filename);
int	rename(const char *old, const char *new);
FILE	*tmpfile(void);
char	*tmpnam(char *s);
int	fclose(FILE *stream);
int	fflush(FILE *stream);
FILE	*fopen(const char *filename, const char *mode);
FILE	*freopen(const char *filename, const char *mode, FILE *stream);
void	setbuf(FILE *stream, char *buf);
int	setvbuf(FILE *stream, char *buf, int mode, size_t size);
int	fprintf(FILE *stream, const char *format, ...);
int	fscanf(FILE *stream, const char *format, ...);
int	printf(const char *format, ...);
int	scanf(const char *format, ...);
int	sprintf(char *s, const char *format, ...);
int	sscanf(char *s, const char *format, ...);
int	vfprintf(FILE *stream, const char *format, va_list arg);
int	vprintf(const char *format, va_list arg);
int	vsprintf(char *s, const char *format, va_list arg);
int	fgetc(FILE *stream);
char	*fgets(char *s, int n, FILE *stream);
int	fputc(int c, FILE *stream);
int	fputs(const char *s, FILE *stream);
char	*gets(char *s);
int	ungetc(int c, FILE *stream);
size_t	fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t	fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int	fgetpos(FILE *stream, fpos_t *pos);
int	fseek(FILE *stream, long offset, int whence);
int	fsetpos(FILE *stream, const fpos_t *pos);
long	ftell(FILE *stream);
void	rewind(FILE *stream);
void	clearerr(FILE *stream);
int	perror(const char *s);
#endif	/* __STDC__ */

#endif	/* _STDIO_HEADER_ */
