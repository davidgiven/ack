/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm1.h	1.5 */
/*
 * variable declarations
 */

#ifdef extern
#define INIT(x) = x
#else
#define INIT(x) /* empty */
#endif

extern short pass INIT(PASS_1);
/* PASS 1, 2 or 3 */
extern short peekc; /* push back symbol (PASS_1) */
extern short unresolved; /* number of unresolved references */
extern long lineno; /* input line number */
extern short hllino; /* high-level language line number */
extern short nerrors; /* terminate at end of pass if set */
extern short sflag INIT(SYM_DEF);
/* -s option (symbol table info) */
extern char* progname; /* for error messages */
extern char* modulename; /* for error messages */
extern common_t* commons; /* header of commons list */

#ifdef ASLD
extern short archmode; /* scanning archives */
extern long archsize; /* size of individual archive entries */
#else
extern short uflag; /* if 1 make undefineds extern */
/* symbol table index for last S_UND */
#endif

#ifdef LISTING
extern short dflag; /* -d option (list mode) */
#endif

#ifdef ASLD
#ifdef RELOCATION
extern short rflag; /* -r option (relocation info) */
#endif /* RELOCATION */
#else
#define rflag 1
extern valu_t relonami;
#endif /* ASLD */

#ifdef THREE_PASS
extern short bflag; /* -b option (no optimizations) */
#endif

extern char* aoutpath INIT("a.out");

extern FILE* input;
extern FILE* tempfile;

extern char* stringbuf; /* contains last string value */
extern int stringlen; /* contains length of last string value */

extern sect_t sect[SECTMAX];

/*
 * specials for the location counter
 */
extern sect_t* DOTSCT; /* &sect[DOTTYP-S_MIN] or NULL */
extern ADDR_T DOTVAL; /* DOTSCT->s_size + DOTSCT->s_base */
extern short DOTTYP; /* S_MIN..S_MAX or S_UND */

extern unsigned short nname; /* Counts name table index in PASS_3 */

extern item_t* hashtab[H_TOTAL];
extern short hashindex; /* see item_search() */

extern item_t* fb_ptr[4 * FB_SIZE];

#ifdef THREE_PASS
#define BITCHUNK (8 * MEMINCR)
extern int nbits;
extern int bitindex; /* bitindex * MEMINCR * 8 + nbits gives
                number of decisions so far
             */
#endif

#ifdef LISTING
extern short listmode; /* -d option for current pass */
extern short listtemp; /* listmode if .list seen */
extern short listflag; /* copied from listtemp at '\n' */
extern short listcolm; /* column on output */
extern short listeoln INIT(1);
/* set by endline, tested by emit1 */
extern FILE* listfile; /* copy of source text */
#endif

#ifndef extern
extern item_t keytab[];
extern struct outhead outhead;
#endif

extern int curr_token;

/* forward function declarations */
/* comm2.y */
extern int yyparse(void);
/* comm4.c */
extern void stop(void);
extern void newmodule(char*);
/* comm5.c */
extern int yylex(void);
extern void putval(int);
extern int getval(int);
extern int nextchar(void);
#ifdef ASLD
extern char* readident(int);
#endif
extern int hash(const char*);
extern item_t* item_search(const char*);
extern void item_insert(item_t*, int);
extern item_t* item_alloc(int);
extern item_t* fb_alloc(int);
extern item_t* fb_shift(int);
/* comm6.c */
extern void newequate(item_t*, int);
extern void newident(item_t*, int);
extern void newlabel(item_t*);
extern void newsect(item_t*);
extern void newbase(valu_t);
extern void newcomm(item_t*, valu_t);
extern void switchsect(int);
extern void align(valu_t);
#ifdef RELOCATION
extern void newrelo(int, int);
#endif
extern long new_string(const char*);
extern void newsymb(const char*, int, int, valu_t);
/* comm7.c */
extern valu_t load(const item_t*);
extern int store(item_t*, valu_t);
extern char* remember(char*);
extern int combine(int, int, int);
#ifdef LISTING
extern int printx(int, valu_t);
extern void listline(int);
#endif
#ifdef THREE_PASS
extern int small(int, int);
#endif
extern void emit1(int);
extern void emit2(int);
extern void emit3(int);
extern void emit4(long);
extern void emitx(valu_t, int);
extern void emit8(int64_t);
extern void emitstr(int);
extern void emitf(int size, int negative);
extern void yyerror(const char*);
extern void nosect(void);
extern void fatal(const char*, ...);
extern void serror(const char*, ...);
extern void warning(const char*, ...);
extern void nofit(void);

/* ========== Machine dependent C declarations ========== */

#include "mach1.c"
