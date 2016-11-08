/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/* offsets of interesting fields in EM-pattern */

#define PO_HASH         0
#define PO_NEXT         1
#define PO_MATCH        3

#define ILLHASH 0177777


/* Commands for codegenerator, in low order 5 bits of byte */

#define DO_NEXTEM       0
#define DO_MATCH        1
#define DO_XMATCH	2
#define DO_XXMATCH	3
#define DO_REMOVE       4
#define DO_DEALLOCATE   5
#define DO_REALLOCATE   6
#define DO_ALLOCATE     7
#define DO_MOVE         10
#define DO_ERASE        11
#define DO_TOKREPLACE   12
#define DO_EMREPLACE    13
#define DO_COST         14
#define DO_RETURN       15
#define DO_COERC        16
#define DO_PRETURN	17
#define DO_RREMOVE	18
#define DO_INSTR	19
#define DO_TEST		20
#define DO_DLINE	21
#define DO_SETCC	22
#define DO_TOSTACK	23
#define DO_KILLREG	24
#define DO_LABDEF	25

#ifndef MAXATT
#define MAXATT TOKENSIZE
#endif

typedef struct instance {
	short in_which;
#               define IN_COPY          1
#		define IN_MEMB		2
#               define IN_RIDENT        3
#               define IN_ALLOC         4
#               define IN_DESCR         5
#               define IN_S_DESCR       6
#               define IN_D_DESCR       7
	short in_info[MAXATT+1];
} inst_t,*inst_p;

typedef struct set {
	short set_size;
	short set_val[SETSIZE];
} set_t,*set_p;

typedef struct {
	short m_set1;           /* number of tokenexpr in move: from */
	short m_expr1;          /* optional expression */
	short m_set2;           /* number of tokenexpr in move: to */
	short m_expr2;          /* optional expression */
	short m_cindex;         /* code index to really do it */
} move_t, *move_p;

typedef struct {
	short t_set;		/* number of tokenexpr in test */
	short t_expr;		/* optional expression */
	short t_cindex;		/* code index to really do it */
} test_t, *test_p;

struct exprnode {
	short ex_operator;
	short ex_lnode;
	short ex_rnode;
};
typedef struct exprnode node_t;
typedef struct exprnode *node_p;

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
#define EX_OR		41
#define EX_XOR		42
#define EX_AND		43
#define EX_ISROM	44
#define EX_TOPELTSIZE	45
#define EX_FALLTHROUGH	46


typedef struct {        /* to stack coercions */
	short c1_texpno;        /* token expression number */
	short c1_expr;		/* boolean expression */
	short c1_prop;		/* property of register needed */
	short c1_codep;         /* code index */
} c1_t,*c1_p;

#ifdef MAXSPLIT
typedef struct {        /* splitting coercions */
	short c2_texpno;        /* token expression number */
	short c2_expr;		/* optional boolean expression */
	short c2_nsplit;        /* split factor */
	short c2_repl[MAXSPLIT];/* replacement instances */
	short c2_codep;         /* code index */
} c2_t,*c2_p;
#endif /* MAXSPLIT */

typedef struct {        /* one to one coercions */
	short c3_texpno;        /* token expression number */
	short c3_expr;		/* boolean expression */
	short c3_prop;          /* property of register needed */
	short c3_repl;          /* replacement instance */
	short c3_codep;         /* code index */
} c3_t,*c3_p;


#define getint(a,b) \
	if ((a=((*(b)++)&BMASK)) >= 128) {\
		a = ((a-128)<<BSHIFT) | (*(b)++&BMASK); \
	}
