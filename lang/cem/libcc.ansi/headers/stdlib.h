/*
 * stdlib.h - standard library
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	_STDLIB_HEADER_
#define	_STDLIB_HEADER_

#ifndef	NULL
#define	NULL		0
#endif	/* NULL */

#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0
#define	RAND_MAX	32767
#define	MB_CUR_MAX	1

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;

#ifndef	_TYPE_SIZE_
#define	_TYPE_SIZE_
typedef unsigned int	size_t;
#endif	/* _TYPE_SIZE_ */

#ifndef	_TYPE_WCHAR_
#define	_TYPE_WCHAR_
typedef int	wchar_t;
#endif	/* _TYPE_WCHAR_ */

#ifdef	__STDC__
double		atof(const char *nptr);
int		atoi(const char *nptr);
long		atol(const char *nptr);
double		strtod(const char *nptr, char **endptr);
long		strtol(const char *nptr, char **endptr, int base);
unsigned long int	strtoul(const char *nptr, char **endptr, int base);
int		rand(void);
void		srand(unsigned int seed);
void		*calloc(size_t nmemb, size_t size);
void		free(void *ptr);
void		*malloc(size_t size);
void		*realloc(void *ptr, size_t size);
void		abort(void);
int		atexit(void (*func)(void));
void		exit(int status);
char		*getenv(const char *name);
int		system(const char *string);
void		*bsearch(const void *key, const void *base,
			size_t nmemb, size_t size,
			int (*compar)(const void *, const void *));
void		qsort(void *base, size_t nmemb, size_t size,
			int (*compar)(const void *, const void *));
int		abs(int j);
div_t		div(int numer, int denom);
long		labs(long j);
ldiv_t		ldiv(long numer, long denom);
int		mblen(const char *s, size_t n);
int		mbtowc(wchar_t *pwc, const char *s, size_t n);
int		wctomb(const char *s, wchar_t wchar);
size_t		mbstowcs(wchar_t *pwcs, const char *s, size_t n);
size_t		wcstombs(char *s, const wchar_t *pwcs, size_t n);
#endif	/* __STDC__ */

#endif	/* _STDLIB_HEADER_ */
