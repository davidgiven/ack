/*
 * stdlib.h - standard library
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

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
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _SIZE_T */

#if	!defined(_WCHAR_T)
#define	_WCHAR_T
typedef char	wchar_t;
#endif	/* _WCHAR_T */

double		atof(const char *_nptr);
int		atoi(const char *_nptr);
long		atol(const char *_nptr);
double		strtod(const char *_nptr, char **_endptr);
long		strtol(const char *_nptr, char **_endptr, int _base);
unsigned long int	strtoul(const char *_nptr, char **_endptr, int _base);
int		rand(void);
void		srand(unsigned int _seed);
void		*calloc(size_t _nmemb, size_t _size);
void		free(void *_ptr);
void		*malloc(size_t _size);
void		*realloc(void *_ptr, size_t _size);
void		abort(void);
int		atexit(void (*_func)(void));
void		exit(int _status);
char		*getenv(const char *_name);
int		system(const char *_string);
void		*bsearch(const void *_key, const void *_base,
			size_t _nmemb, size_t _size,
			int (*_compar)(const void *, const void *));
void		qsort(void *_base, size_t _nmemb, size_t _size,
			int (*_compar)(const void *, const void *));
int		abs(int _j);
div_t		div(int _numer, int _denom);
long		labs(long _j);
ldiv_t		ldiv(long _numer, long _denom);
int		mblen(const char *_s, size_t _n);
int		mbtowc(wchar_t *_pwc, const char *_s, size_t _n);
int		wctomb(char *_s, wchar_t _wchar);
size_t		mbstowcs(wchar_t *_pwcs, const char *_s, size_t _n);
size_t		wcstombs(char *_s, const wchar_t *_pwcs, size_t _n);

#endif	/* _STDLIB_H */
