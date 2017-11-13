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

/* codegen.c */
unsigned codegen(byte *, int, int, unsigned, int);
void readcodebytes(void);
#ifdef TABLEDEBUG
void initlset(char *);
void termlset(void);
#endif
/* compute.c */
string mystrcpy(string);
string tostring(word);
/* fillem.c */
void in_init(char *);
void in_start(void);
void fillemlines(void);
void swtxt(void);
/* gencode.c */
void out_init(char *);
void out_finish(void);
void tstoutput(void);
/* nextem.c */
byte *nextem(int);
/* reg.c */
void chrefcount(int, int, int);
int getrefcount(int, int);
void erasereg(int);
void cleanregs(void);
void chkregs(void);
/* salloc.c */
string myalloc(int);
void myfree(string);
void popstr(int);
char *salloc(int);
void garbage_collect(void);
/* subr.c */
void itokcost(void);
void error(const char *s, ...);
void fatal(const char *s, ...);
