/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/*
 * Miscellaneous sizes, tunable 
 */

#include <local.h>
#if BIGMACHINE
#define BORS(x,y) x
#else
#define BORS(x,y) y
#endif

#define MAXREGS BORS(40,30)
#define MAXPROPS BORS(30,20)
#define MAXTOKENS BORS(100,60)
#define MAXATT 6
#define MAXSETS BORS(200,100)
#define MAXINSTR BORS(200,125)
#define MAXSTRINGS BORS(300,200)
#define MAXNODES BORS(1200,350)
#define EMPATMAX BORS(20,10)
#define MAXPATTERNS BORS(40,30)
#define MAXALLREG 5
#define MAXINSTANCES BORS(500,300)
#define MAXMOVES BORS(40,20)
#define MAXTESTS 10
#define MAXSTACKS BORS(30,20)
#define MAXCOERCS BORS(50,30)
#define MAXSPLCOERC BORS(20,10)
#define MAXSPLIT 3
#define MAXPATBYTES BORS(14000,7000)
#define MAXREGVAR 10
#define MAXSOURCELINES 7000
#define MAXPROCARG 10

/* end of tunable constants */

#define TOKPATMAX 7

#define SZOFSET(n) (((n)+15)/16)

#define SETSIZE SZOFSET(MAXREGS+MAXTOKENS)

#define NEXT(n,max,string) (n<max? n++ : tabovf(string))
#define NEW(x,y) x=(y*)myalloc(sizeof(*(x)))

#include <em_spec.h>
#define op_lab	(sp_lmnem + 1)
