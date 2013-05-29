/*
 * stdio.h - input/output definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef _STDIO_H
#define	_STDIO_H

#include <stddef.h>

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

#define	SEEK_SET    0
#define	SEEK_CUR    1
#define	SEEK_END    2

#define	stdin       (&__stdin)
#define	stdout      (&__stdout)
#define	stderr      (&__stderr)

#define	BUFSIZ      1024
#define	EOF         (-1)

#define	FOPEN_MAX	20

#define	FILENAME_MAX 255
#define	TMP_MAX     999
#define	L_tmpnam    (sizeof("/tmp/") + 15)

typedef long int fpos_t;

extern FILE	*__iotab[FOPEN_MAX];
extern FILE	__stdin, __stdout, __stderr;

extern int	remove(const char *_filename);
extern int	rename(const char *_old, const char *_new);
extern FILE	*tmpfile(void);
extern char	*tmpnam(char *_s);
extern int	fclose(FILE *_stream);
extern int	fflush(FILE *_stream);
extern FILE	*fopen(const char *_filename, const char *_mode);
extern FILE	*freopen(const char *_filename, const char *_mode, FILE *_stream);
extern void	setbuf(FILE *_stream, char *_buf);
extern int	setvbuf(FILE *_stream, char *_buf, int _mode, size_t _size);
extern int	fprintf(FILE *_stream, const char *_format, ...);
extern int	fscanf(FILE *_stream, const char *_format, ...);
extern int	printf(const char *_format, ...);
extern int	scanf(const char *_format, ...);
extern int	sprintf(char *_s, const char *_format, ...);
extern int	snprintf(char *_s, size_t _len, const char *_format, ...);
extern int	sscanf(const char *_s, const char *_format, ...);
extern int	vfprintf(FILE *_stream, const char *_format, char *_arg);
extern int	vprintf(const char *_format, char *_arg);
extern int	vsprintf(char *_s, const char *_format, char *_arg);
extern int	vsnprintf(char *_s, size_t _len, const char *_format, char *_arg);
extern int	fgetc(FILE *_stream);
extern char	*fgets(char *_s, int _n, FILE *_stream);
extern int	fputc(int _c, FILE *_stream);
extern int	fputs(const char *_s, FILE *_stream);
extern int	getc(FILE *_stream);
extern int	getchar(void);
extern char	*gets(char *_s);
extern int	putc(int _c, FILE *_stream);
extern int	putchar(int _c);
extern int	puts(const char *_s);
extern int	ungetc(int _c, FILE *_stream);
extern size_t	fread(void *_ptr, size_t _size, size_t _nmemb, FILE *_stream);
extern size_t	fwrite(const void *_ptr, size_t _size, size_t _nmemb, FILE *_stream);
extern int	fgetpos(FILE *_stream, fpos_t *_pos);
extern int	fseek(FILE *_stream, long _offset, int _whence);
extern int	fsetpos(FILE *_stream, fpos_t *_pos);
extern long	ftell(FILE *_stream);
extern void	rewind(FILE *_stream);
extern void	clearerr(FILE *_stream);
extern int	feof(FILE *_stream);
extern int	ferror(FILE *_stream);
extern void	perror(const char *_s);

extern int __fillbuf(FILE *_stream);
extern int __flushbuf(int _c, FILE *_stream);

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

/* Non-standard extensions */

extern int fileno(FILE *_stream);
extern FILE* fdopen(int fildes, const char *type);
#define	fileno(stream) ((stream)->_fd)

#endif	/* _STDIO_H */
