/* $Header$ */

/*
 * Miscellaneous sizes, tunable 
 */

#define MAXREGS 40
#define MAXPROPS 30
#define MAXTOKENS 100
#define MAXATT 6
#define MAXSETS 150
#define MAXINSTR 200
#define MAXSTRINGS 250
#define MAXNODES 500
#define EMPATMAX 20
#define MAXPATTERNS 40
#define MAXALLREG 5
#define MAXINSTANCES 500
#define MAXMOVES 40
#define MAXTESTS 10
#define MAXSTACKS 30
#define MAXCOERCS 50
#define MAXSPLCOERC 20
#define MAXSPLIT 3
#define MAXPATBYTES 7000
#define MAXREGVAR 10
#define MAXSOURCELINES 6000

/* end of tunable constants */

#define TOKPATMAX 7

#define SZOFSET(n) (((n)+15)/16)

#define SETSIZE SZOFSET(MAXREGS+MAXTOKENS)

#define NEXT(n,max,string) (n<max? n++ : tabovf(string))
#define NEW(x,y) x=(y*)myalloc(sizeof(*(x)))
