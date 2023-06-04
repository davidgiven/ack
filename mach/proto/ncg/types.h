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
extern unsigned codegen(byte *, int, int, unsigned, int);
extern void readcodebytes(void);
#ifdef TABLEDEBUG
extern void initlset(char *);
extern void termlset(void);
#endif
/* compute.c */
extern string mystrcpy(string);
extern string tostring(word);
/* fillem.c */
extern void in_init(char *);
extern void in_start(void);
extern void fillemlines(void);
extern void swtxt(void);
extern void dopseudo(void);
/* gencode.c */
extern void out_init(char *);
extern void out_finish(void);
extern void tstoutput(void);
/* nextem.c */
extern byte *nextem(int);
/* reg.c */
extern void chrefcount(int, int, int);
extern int getrefcount(int, int);
extern void erasereg(int);
extern void cleanregs(void);
extern void chkregs(void);
/* salloc.c */
extern string myalloc(int);
extern void myfree(string);
extern void popstr(int);
extern char *salloc(int);
extern void garbage_collect(void);
/* subr.c */
extern void itokcost(void);
extern void error(const char *s, ...);
extern void fatal(const char *s, ...);
