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
#include <stdarg.h>
#include <ack/config.h>

/* Public stdio entry points. */

typedef struct FILE FILE;

#define	EOF         (-1)

#define	SEEK_SET    0
#define	SEEK_CUR    1
#define	SEEK_END    2

typedef long int fpos_t;

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
extern int	vfscanf(FILE *_stream, const char *_format, va_list ap);
extern int	printf(const char *_format, ...);
extern int	scanf(const char *_format, ...);
extern int	sprintf(char *_s, const char *_format, ...);
extern int	snprintf(char *_s, size_t _len, const char *_format, ...);
extern int	sscanf(const char *_s, const char *_format, ...);
extern int	vfprintf(FILE *_stream, const char *_format, char *_arg);
extern int	vprintf(const char *_format, char *_arg);
extern int	vsprintf(char *_s, const char *_format, va_list ap);
extern int	vsnprintf(char *_s, size_t _len, const char *_format, va_list ap);
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

#define	getchar()	getc(stdin)
#define	putchar(c)	putc(c, stdout)

/* Internal function used in several places which is approximately itoa(). */
extern char *_i_compute(unsigned long val, int base, char *s, int nrdigits);

#if ACKCONF_WANT_EMULATED_FILE
#include <ack/emufile.h>
#else
#include <ack/file.h>
#endif

#endif	/* _STDIO_H */
