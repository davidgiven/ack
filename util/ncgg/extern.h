/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

extern int wordsize;
extern int pointersize;
extern int nregs;
extern int nprops;
extern int ntokens;
extern int nsets;
extern int ninstr;
extern int empatlen;
extern int emrepllen;
extern int tokrepllen;
extern int emmnem[];
extern int empatexpr;
extern int codeindex;
extern int tokpatlen;
extern int maxtokpatlen;
extern int maxempatlen;
extern int maxemrepllen;
extern int maxtokrepllen;
extern int tokpatro[];
extern int tokpatset[];
extern int nallreg;
extern int allreg[];
extern int cursetno;
extern int allsetno;
extern int inproc;
extern int callproc;
extern int procarg[];
extern int fc1,fc2,fc3,fc4;
extern int maxmembers;
extern int regclass;
extern int maxtokensize;
extern int nprocargs, maxprocargs;
extern int use_tes;

/* genenerated files */
extern void enterkeyw(void);
extern int yyparse(void);

/* emlookup.c */
extern void initemhash(void);

/* error.c */
extern void fatal(const char* s, ...);
extern void error(const char* s, ...);
extern int tabovf(char *string);

/* output.c */
extern void errorexit(void);
extern void initio(void);
extern void finishio(void);
extern void statistics(void);

/* strlookup.c */
extern int strlookup(char *str);
