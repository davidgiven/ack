#define SZADDR EM_PSIZE
#define SZWORD EM_WSIZE
#define SZLONG 4
#define SZREAL 8
#define SZPROC SZADDR+SZADDR

#if SZWORD==2
#if SZADDR==2
#define FIRSTIBOFFSET 30 /* offset from .HTOP to main's LB */
#else
#define FIRSTIBOFFSET 50
#endif
#else
#define FIRSTIBOFFSET 52 
#endif
#define FSTAMPOFFSET FIRSTIBOFFSET+SZWORD
/* the following four definitions are offsets to the file pointers */
#define FILEOFFSET SZWORD+SZWORD+SZADDR+SZWORD+SZWORD+SZADDR+SZADDR+SZWORD+\
		   SZADDR+SZADDR+SZWORD+SZWORD+SZADDR
#define STINOFFSET FIRSTIBOFFSET+FILEOFFSET
#define STOUTOFFSET STINOFFSET+SZADDR
#define STBACKOFFSET STOUTOFFSET+SZADDR
#define ENTRYOFFSET SZADDR+SZWORD

#if SZWORD==2
#define PUTTVARSPACE 150 /* space, or greater used for locals in PUTT */
#define GETTVARSPACE 350 /* exactly (!) space used for locals in GETT */
#define LLC ldc 	 /* for loading bit patterns */
#else
#define PUTTVARSPACE 212
#define GETTVARSPACE 432
#define LLC loc
#endif
#define HTOP 500   /* this must agree with what the compiler produces */
#define A68STAMP 13476 /* this must agree with version in a68sdec.p */
#define PASCALSTAMP 0 /* must match what the pascal compiler puts down */
#define PUTSTAMP -1
#define GETSTAMP -2

/* this will only work if SZADDR = SWORD*2 or if SZADDR = SZWORD */

#if SZADDR == SZWORD
#define LFL lol
#define SFL stl
#define LFE loe
#define SFE ste
#define LFF lof
#define SFF stf
#else
#define LFL ldl
#define SFL sdl
#define LFE lde
#define SFE sde
#define LFF ldf
#define SFF sdf
#endif

 mes 2,SZWORD,SZADDR

