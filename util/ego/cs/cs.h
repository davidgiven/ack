/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
typedef short		valnum;
typedef struct entity 	*entity_p;
typedef struct avail 	*avail_p;
typedef struct token	*token_p;
typedef struct occur	*occur_p;

struct token {
	valnum	tk_vn;
	offset	tk_size;
	line_p	tk_lfirst;	/* Textually first instruction, involved
			 	 * in pushing this token.
				 */
};

	/* We distinguish these entities. */
#define ENCONST		0
#define ENLOCAL		1
#define ENEXTERNAL	2
#define ENINDIR		3
#define ENOFFSETTED	4
#define ENALOCAL	5
#define ENAEXTERNAL	6
#define ENAOFFSETTED	7
#define ENALOCBASE	8
#define ENAARGBASE	9
#define ENPROC		10
#define ENFZER		11
#define ENARRELEM	12
#define ENLOCBASE	13
#define ENHEAPPTR	14
#define ENIGNMASK	15

struct entity {
	valnum	en_vn;
	bool	en_static;
	byte	en_kind;		/* ENLOCAL, ENEXTERNAL, etc.	*/
	offset	en_size;
	union {
		offset	en__val;	/* ENCONST.			*/
		offset	en__loc;	/* ENLOCAL, ENALOCAL.		*/
		obj_p	en__ext;	/* ENEXTERNAL, ENAEXTERNAL.	*/
		valnum	en__ind;	/* ENINDIR.			*/
		struct {
			valnum	en__base;
			offset	en__off;
		} en_offs;		/* ENOFFSETTED, ENAOFFSETTED.	*/
		offset	en__levels;	/* ENALOCBASE, ENAARGBASE.	*/
		proc_p	en__pro;	/* ENPROC.			*/
		struct {
			valnum	en__arbase;
			valnum	en__index;
			valnum	en__adesc;
		} en_arr;		/* ENARRELEM.			*/
	} en_inf;
};

	/* Macros to increase ease of use. */
#define en_val		en_inf.en__val
#define en_loc		en_inf.en__loc
#define en_ext		en_inf.en__ext
#define en_ind		en_inf.en__ind
#define en_base		en_inf.en_offs.en__base
#define en_off		en_inf.en_offs.en__off
#define en_levels	en_inf.en__levels
#define en_pro		en_inf.en__pro
#define en_arbase	en_inf.en_arr.en__arbase
#define en_index	en_inf.en_arr.en__index
#define en_adesc	en_inf.en_arr.en__adesc

struct occur {
	line_p		oc_lfirst;  /* First instruction of expression. */
	line_p		oc_llast;   /* Last one. */
	bblock_p	oc_belongs; /* Basic block it belongs to. */
};
	    
	/* We distinguish these groups of instructions. */
#define SIMPLE_LOAD	0
#define EXPENSIVE_LOAD	1
#define LOAD_ARRAY	2
#define STORE_DIRECT	3
#define STORE_INDIR	4
#define STORE_ARRAY	5
#define UNAIR_OP	6
#define BINAIR_OP	7
#define TERNAIR_OP	8
#define KILL_ENTITY	9
#define SIDE_EFFECTS	10
#define FIDDLE_STACK	11
#define IGNORE		12
#define HOPELESS	13
#define BBLOCK_END	14

struct avail {
	avail_p	av_before;	/* Ptr to earlier discovered expressions. */
	byte	av_instr;	/* Operator instruction. */
	offset	av_size;
	line_p	av_found;	/* Line where expression is first found. */
	lset	av_occurs;	/* Set of recurrences of expression. */
	entity_p av_saveloc;	/* Local where result is put in. */
	valnum	av_result;
	union {
		valnum	av__operand;		/* EXPENSIVE_LOAD, UNAIR_OP. */
		struct {
			valnum	av__oleft;
			valnum	av__oright;
		} av_2;				/* BINAIR_OP. */
		struct {
			valnum	av__ofirst;
			valnum	av__osecond;
			valnum	av__othird;
		} av_3;				/* TERNAIR_OP. */
	} av_o;
};

	/* Macros to increase ease of use. */
#define av_operand	av_o.av__operand
#define av_oleft	av_o.av_2.av__oleft
#define av_oright	av_o.av_2.av__oright
#define av_ofirst	av_o.av_3.av__ofirst
#define av_osecond	av_o.av_3.av__osecond
#define av_othird	av_o.av_3.av__othird

extern int Scs; /* Number of optimizations found. */
