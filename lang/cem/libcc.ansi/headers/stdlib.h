/*
 * stdlib.h - standard library
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(_STDLIB_H)
#define	_STDLIB_H

#define	NULL		((void *)0)

#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0
#define	RAND_MAX	32767
#define	MB_CUR_MAX	1

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;

#if	!defined(_SIZE_T)
#define	_SIZE_T
#if	_EM_WSIZE  == _EM_PSIZE
typedef unsigned int	size_t;		/* type returned by sizeof */
#else
typedef unsigned long	size_t;		/* type returned by sizeof */
#endif
#endif	/* _SIZE_T */

#if	!defined(_WCHAR_T)
#define	_WCHAR_T
typedef char	wchar_t;
#endif	/* _WCHAR_T */

double		atof(const char *__nptr);
int		atoi(const char *__nptr);
long		atol(const char *__nptr);
double		strtod(const char *__nptr, char **__endptr);
long		strtol(const char *__nptr, char **__endptr, int __base);
unsigned long int	strtoul(const char *__nptr, char **__endptr, int __base);
int		rand(void);
void		srand(unsigned int __seed);
void		*calloc(size_t __nmemb, size_t __size);
void		free(void *__ptr);
void		*malloc(size_t __size);
void		*realloc(void *__ptr, size_t __size);
void		abort(void);
int		atexit(void (*__func)(void));
void		exit(int __status);
char		*getenv(const char *__name);
int		system(const char *__string);
void		*bsearch(const void *__key, const void *__base,
			size_t __nmemb, size_t __size,
			int (*__compar)(const void *, const void *));
void		qsort(void *__base, size_t __nmemb, size_t __size,
			int (*__compar)(const void *, const void *));
int		abs(int __j);
div_t		div(int __numer, int __denom);
long		labs(long __j);
ldiv_t		ldiv(long __numer, long __denom);
int		mblen(const char *__s, size_t __n);
int		mbtowc(wchar_t *__pwc, const char *__s, size_t __n);
int		wctomb(char *__s, wchar_t __wchar);
size_t		mbstowcs(wchar_t *__pwcs, const char *__s, size_t __n);
size_t		wcstombs(char *__s, const wchar_t *__pwcs, size_t __n);

#endif	/* _STDLIB_H */
