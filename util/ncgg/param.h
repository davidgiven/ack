/* $Header$ */

/*
 * Miscellaneous sizes, tunable 
 */

#define MAXREGS 40
#define MAXPROPS 30
#define MAXTOKENS 60
#define MAXATT 3
#define MAXSETS 100
#define MAXINSTR 100
#define MAXSTRINGS 250
#define MAXNODES 300
#define EMPATMAX 20
#define MAXPATTERNS 20
#define MAXALLREG 5
#define MAXINSTANCES 300
#define MAXMOVES 20
#define MAXTESTS 10
#define MAXSTACKS 30
#define MAXCOERCS 25
#define MAXSPLCOERC 20
#define MAXSPLIT 2
#define MAXPATBYTES 7000
#define MAXREGVAR 8
#define MAXSOURCELINES 4000

/* end of tunable constants */

#define TOKPATMAX 7

#define SZOFSET(n) (((n)+15)/16)

#define SETSIZE SZOFSET(MAXREGS+MAXTOKENS)

#define NEXT(n,max,string) (n<max? n++ : tabovf(string))
#define NEW(x,y) x=(y*)myalloc(sizeof(*(x)))
