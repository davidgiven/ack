/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef TEM_WSIZE
TEM_WSIZE should be defined at this point
#endif
#ifndef TEM_PSIZE
TEM_PSIZE should be defined at this point
#endif
#if TEM_WSIZE>4 || TEM_PSIZE>4
Implementation will not be correct unless a long integer
has more then 4 bytes of precision.
#endif

typedef char byte;
typedef char * string;

#if TEM_WSIZE>2 || TEM_PSIZE>2
#define full            long
#else
#define full		int
#endif

#define word long
#ifndef WRD_FMT
#define WRD_FMT "%ld"
#endif /* WRD_FMT */

/* compute.c */
string mystrcpy(string);
string tostring(word);
/* salloc.c */
string myalloc(int);
void myfree(string);
void popstr(int);
char *salloc(int);
void garbage_collect(void);
