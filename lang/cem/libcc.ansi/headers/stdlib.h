/*
 * stdlib.h - standard library
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef _STDLIB_H
#define	_STDLIB_H

#include <stddef.h>

#define	EXIT_FAILURE 1
#define	EXIT_SUCCESS 0
#define	RAND_MAX 32767
#define	MB_CUR_MAX sizeof(wchar_t)

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;

extern double atof(const char *_nptr);
extern int atoi(const char *_nptr);
extern long atol(const char *_nptr);
extern double strtod(const char *_nptr, char **_endptr);
extern long strtol(const char *_nptr, char **_endptr, int _base);
extern unsigned long strtoul(const char *_nptr, char **_endptr, int _base);
extern int rand(void);
extern void srand(unsigned int _seed);
extern void* calloc(size_t _nmemb, size_t _size);
extern void free(void *_ptr);
extern void* malloc(size_t _size);
extern void* realloc(void *_ptr, size_t _size);
extern void abort(void);
extern int atexit(void (*_func)(void));
extern void exit(int _status);
extern void _Exit(int _status);
extern char* getenv(const char *_name);
extern int system(const char *_string);
extern void* bsearch(const void *_key, const void *_base,
			size_t _nmemb, size_t _size,
			int (*_compar)(const void *, const void *));
extern void qsort(void *_base, size_t _nmemb, size_t _size,
			int (*_compar)(const void *, const void *));
extern int abs(int _j);
extern div_t div(int _numer, int _denom);
extern long labs(long _j);
extern ldiv_t ldiv(long _numer, long _denom);
extern int mblen(const char *_s, size_t _n);
extern int mbtowc(wchar_t *_pwc, const char *_s, size_t _n);
extern int wctomb(char *_s, wchar_t _wchar);
extern size_t mbstowcs(wchar_t *_pwcs, const char *_s, size_t _n);
extern size_t wcstombs(char *_s, const wchar_t *_pwcs, size_t _n);

/* Extensions (not part of the standard) */

#define atof(n)   strtod(n, (char **)NULL)
#define atoi(n)   ((int)strtol(n, (char **)NULL, 10))
#define atol(n)   strtol(n, (char **)NULL, 10)
#define atoll(n)  strtoll(n, (char **)NULL, 10)
#define mblen(s, n)   mbtowc((wchar_t *)0, s, n)

#endif
