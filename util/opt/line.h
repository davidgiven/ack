/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#define NARGBYTES	14
struct argbytes {
	argb_p	ab_next;
	short	ab_index;
	char	ab_contents[NARGBYTES];
};

typedef struct {
	sym_p	av_sp;
	offset	av_offset;
} s_a_val;

typedef struct {
	short	ac_length;
	argb_t	ac_con;
} s_a_con;

typedef union {
	offset	a_offset;
	num_p	a_np;
	sym_p	a_sp;
	s_a_val	a_val;
	argb_t	a_string;
	s_a_con	a_con;
} un_a_a;

struct arg {
	arg_p	a_next;
	short	a_typ;
	un_a_a	a_a;
};

/* possible values for .a_typ
 */

#define ARGOFF	0
#define ARGNUM	1
#define ARGSYM	2
#define ARGVAL	3
#define ARGSTR	4
#define ARGICN	5
#define ARGUCN	6
#define ARGFCN	7

typedef struct {
	sym_p	lasv_sp;
	short	lasv_short;
} s_la_sval;

typedef struct {
	sym_p	lalv_sp;
	offset	lalv_offset;
} s_la_lval;

typedef union {
	short	la_short;
	offset	la_offset;
	num_p	la_np;
	sym_p	la_sp;
	s_la_sval la_sval;
	s_la_lval la_lval;
	arg_p	la_arg;
} un_l_a;

struct line {
	line_p		l_next;		/* maintains linked list */
	byte		l_instr;	/* instruction number */
	byte		l_optyp;	/* specifies what follows */
	un_l_a		l_a;
};

/* Possible values for .l_optyp */

#define OPNO		0	/* no operand */
#define OPSHORT		1	/* 16 bit number */
#define OPOFFSET	2	/* 16 or 32 bit number */
#define OPNUMLAB	3	/* local label for branches */
#define OPSYMBOL	4	/* global label or procedurename */
#define OPSVAL		5	/* symbol + 16 bit constant */
#define OPLVAL		6	/* symbol + 16 or 32 bit constant */
#define OPLIST		7	/* operand list for some pseudos */
#define OPMINI		8	/* start of minis */

#define Z_OPMINI	(OPMINI+100)	/* tunable */

#define CANMINI(x) ((x)>=OPMINI-Z_OPMINI && (x)<256-Z_OPMINI)
