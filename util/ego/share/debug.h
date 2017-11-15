/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* D E B U G G I N G   T O O L S */

/* TEMPORARY: */
/* #define DEBUG */

extern int		linecount;	/* # lines in this file */
extern bool		verbose_flag;  /* generate verbose output ? */

/* VARARGS 1 */
void error(const char *, ...);


#ifdef TRACE
void OUTTRACE(const char *, int);
#else
#define OUTTRACE(s,n)
#endif
#ifdef VERBOSE
void OUTVERBOSE(const char *, int, int);
#else
#define OUTVERBOSE(s,n1,n2)
#endif
#ifdef DEBUG

/* Some (all?) Unix debuggers don't particularly like
 * static procedures and variables. Therefor we make everything
 * global when debugging.
 */

#define STATIC

#define assert(x) if(!(x)) badassertion(__FILE__,__LINE__)

void VI(short);
void VL(line_p);
void VD(dblock_p);
void VA(short *);
void VO(obj_p);
void VP(proc_p);



#else /*DEBUG*/

#define assert(b)

#define VI(i)
#define VL(l)
#define VD(d)
#define VA(a)
#define VO(o)
#define VP(p)


#define STATIC static
#endif
