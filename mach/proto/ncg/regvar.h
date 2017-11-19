/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

struct regvar {
	struct regvar  *rv_next;
	long		rv_off;
	int		rv_size;
	int		rv_type;
	int		rv_score;
	int		rv_reg;
};

struct regassigned {
	struct regvar  *ra_rv;
	int		ra_score;
};

extern int nregvar[];
extern struct regassigned *regassigned[];

struct regvar *linkreg(long, int, int, int);
void tryreg(struct regvar *, int);
void fixregvars(int);
int isregvar(long);
#ifdef REGLAP
int isregvar_size(long, int);
#endif
int isregtyp(long);
void unlinkregs(void);

#ifdef REGLAP
#define PICK_REGVAR(off, size) isregvar_size(off, size)
#else
#define PICK_REGVAR(off, size) isregvar(off)
#endif

/* machine dependent */
int regscore(long, int, int, int, int);
void i_regsave(void);
void regsave(const char *, long, int);
void f_regsave(void);
void regreturn(void);
