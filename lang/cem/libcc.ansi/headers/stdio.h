/*
 * stdio.h - input/output definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(__STDIO_HEADER__)
#define	__STDIO_HEADER__

/*
 * Focus point of all stdio activity.
 */
typedef struct __iobuf {
	int		_count;
	int		_fd;
	int		_flags;
	int		_bufsiz;
	char		*_tname;
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
#if	!defined(NULL)
#define	NULL		0
#endif	/* NULL */
#if	!defined(EOF)
#define	EOF		(-1)
#endif	/* EOF */

#define	FOPEN_MAX	20

#if	defined(__BSD4_2)
#define	FILENAME_MAX	255
#else
#define	FILENAME_MAX	14
#endif	/* __BSD4_2 */
#define	TMP_MAX		999
#define	L_tmpnam	(sizeof("/tmp/") + 15)

#if	!defined(__TYPE_FPOS__)
#define	__TYPE_FPOS__
typedef long int	fpos_t;
#endif	/* __TYPE_FPOS__ */

#if	!defined(__TYPE_SIZE__)
#define	__TYPE_SIZE__
typedef unsigned int	size_t;
#endif	/* __TYPE_SIZE__ */

extern FILE	*__iotab[FOPEN_MAX];
extern FILE	__stdin, __stdout, __stderr;

int	remove(const char *__filename);
int	rename(const char *__old, const char *__new);
FILE	*tmpfile(void);
char	*tmpnam(char *__s);
int	fclose(FILE *__stream);
int	fflush(FILE *__stream);
FILE	*fopen(const char *__filename, const char *__mode);
FILE	*freopen(const char *__filename, const char *__mode, FILE *__stream);
void	setbuf(FILE *__stream, char *__buf);
int	setvbuf(FILE *__stream, char *__buf, int __mode, size_t __size);
int	fprintf(FILE *__stream, const char *__format, ...);
int	fscanf(FILE *__stream, const char *__format, ...);
int	printf(const char *__format, ...);
int	scanf(const char *__format, ...);
int	sprintf(char *__s, const char *__format, ...);
int	sscanf(char *__s, const char *__format, ...);
int	vfprintf(FILE *__stream, const char *__format, void *__arg);
int	vprintf(const char *__format, void *__arg);
int	vsprintf(char *__s, const char *__format, void *__arg);
int	fgetc(FILE *__stream);
char	*fgets(char *__s, int __n, FILE *__stream);
int	fputc(int __c, FILE *__stream);
int	fputs(const char *__s, FILE *__stream);
int	getc(FILE *__stream);
int	getchar(void);
char	*gets(char *__s);
int	putc(int __c, FILE *__stream);
int	putchar(int __c);
int	puts(const char *__s);
int	ungetc(int __c, FILE *__stream);
size_t	fread(void *__ptr, size_t __size, size_t __nmemb, FILE *__stream);
size_t	fwrite(const void *__ptr, size_t __size, size_t __nmemb, FILE *__stream);
int	fgetpos(FILE *__stream, fpos_t *__pos);
int	fseek(FILE *__stream, long __offset, int __whence);
int	fsetpos(FILE *__stream, fpos_t *__pos);
long	ftell(FILE *__stream);
void	rewind(FILE *__stream);
void	clearerr(FILE *__stream);
int	feof(FILE *__stream);
int	ferror(FILE *__stream);
void	perror(const char *__s);

int __fillbuf(FILE *__stream);
int __flushbuf(int __c, FILE *__stream);


#define	getchar()	getc(stdin)
#define	putchar(c)	putc(c,stdout)
#define	getc(p)		(--(p)->_count >= 0 ? (int) (*(p)->_ptr++) : \
				__fillbuf(p))
#define	putc(c, p)	(--(p)->_count >= 0 ? \
			 (int) (*(p)->_ptr++ = (c)) : \
			 __flushbuf((c),(p)))

#define	feof(p)		(((p)->_flags & _IOEOF) != 0)
#define	ferror(p)	(((p)->_flags & _IOERR) != 0)

#if	defined(_POSIX_SOURCE)
int fileno(FILE *__stream);
#define	fileno(stream)		((stream)->_fd)
#endif	/* _POSIX_SOURCE */

#endif	/* __STDIO_HEADER__ */
