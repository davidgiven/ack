/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* offsets of interesting fields in EM-pattern */

#define PO_HASH         0
#define PO_NEXT         1
#define PO_MATCH        3

#define ILLHASH 0177777

/* Escapes in printstrings */

#define PR_TOK          '\001'
#define PR_TOKFLD       '\002'
#define PR_EMINT        '\003'
#define PR_EMSTR        '\004'
#define PR_ALLREG       '\005'
#define PR_SUBREG       '\006'
/*
 * In case this list gets longer remember to keep out printable nonprintables
 * like \t \n \r and the like.
 */

/* Commands for codegenerator, in low order 5 bits of byte */

#define DO_NEXTEM       0
#define DO_MATCH        1
#define DO_XMATCH	2
#define DO_XXMATCH	3
#define DO_REMOVE       4
#define DO_DEALLOCATE   5
#define DO_REALLOCATE   6
#define DO_ALLOCATE     7
#define DO_LOUTPUT      8
#define DO_ROUTPUT      9
#define DO_MOVE         10
#define DO_ERASE        11
#define DO_TOKREPLACE   12
#define DO_EMREPLACE    13
#define DO_COST         14
#define DO_RETURN       15
#define DO_COERC        16
#define DO_PRETURN	17
#define DO_RREMOVE	18

typedef struct instance {
	int in_which;
#               define IN_COPY          1
#               define IN_RIDENT        2
#               define IN_ALLOC         3
#               define IN_DESCR         4
#		define IN_REGVAR	5
	int in_info[TOKENSIZE+1];
} inst_t,*inst_p;

typedef struct {
	int     c_size;                 /* index in enode-table */
	int     c_time;                 /* dito */
} cost_t,*cost_p;

typedef struct {
	int m_set1;             /* number of tokenexpr in move: from */
	int m_expr1;            /* optional expression */
	int m_set2;             /* number of tokenexpr in move: to */
	int m_expr2;            /* optional expression */
	int m_cindex;           /* code index to really do it */
	cost_t m_cost;          /* associated cost */
} move_t, *move_p;

typedef struct {
	int set_size;
	short set_val[SETSIZE];
} set_t,*set_p;

struct exprnode {
	short ex_operator;
	short ex_lnode;
	short ex_rnode;
};
typedef struct exprnode node_t;
typedef struct exprnode *node_p;

typedef struct {        /* to stack coercions */
	int c1_texpno;          /* token expression number */
	int c1_expr;		/* boolean expression */
	int c1_prop;		/* property of register needed */
	int c1_codep;           /* code index */
	cost_t c1_cost;		/* cost involved */
} c1_t,*c1_p;

#ifdef MAXSPLIT
typedef struct {        /* splitting coercions */
	int c2_texpno;          /* token expression number */
	int c2_nsplit;          /* split factor */
	int c2_repl[MAXSPLIT];  /* replacement instances */
	int c2_codep;           /* code index */
} c2_t,*c2_p;
#endif /* MAXSPLIT */

typedef struct {        /* one to one coercions */
	int c3_texpno;          /* token expression number */
	int c3_prop;            /* property of register needed */
	int c3_repl;            /* replacement instance */
	int c3_codep;           /* code index */
} c3_t,*c3_p;

/*
 * contents of .ex_operator
 */

#define EX_TOKFIELD     0
#define EX_ARG          1
#define EX_CON          2
#define EX_ALLREG       3
#define EX_SAMESIGN     4
#define EX_SFIT         5
#define EX_UFIT         6
#define EX_ROM          7
#define EX_NCPEQ        8
#define EX_SCPEQ        9
#define EX_RCPEQ        10
#define EX_NCPNE        11
#define EX_SCPNE        12
#define EX_RCPNE        13
#define EX_NCPGT        14
#define EX_NCPGE        15
#define EX_NCPLT        16
#define EX_NCPLE        17
#define EX_OR2          18
#define EX_AND2         19
#define EX_PLUS         20
#define EX_CAT          21
#define EX_MINUS        22
#define EX_TIMES        23
#define EX_DIVIDE       24
#define EX_MOD          25
#define EX_LSHIFT       26
#define EX_RSHIFT       27
#define EX_NOT          28
#define EX_COMP         29
#define EX_COST         30
#define EX_STRING       31
#define EX_DEFINED      32
#define EX_SUBREG       33
#define EX_TOSTRING     34
#define EX_UMINUS       35
#define EX_REG          36
#define EX_LOWW		37
#define EX_HIGHW	38
#define EX_INREG	39
#define EX_REGVAR	40



#define getint(a,b) \
	if ((a=((*(b)++)&BMASK)) >= 128) {\
		a = ((a-128)<<BSHIFT) | (*(b)++&BMASK); \
	}
