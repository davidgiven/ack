/* When generating assembly : */

#define 	ARITH_FMT	"%ld"
#define 	INT_FMT		"%d"

/* When generating object code : */

#define		CONST		0
#define		REGISTER	1
#define		AUTO_DEC	2
#define		AUTO_INC	3
#define		REG_DEF		4
#define		IND_REG		5
#define		LABEL		6
#define		L_ILB		7

#define		TRUE		1
#define		FALSE		0

#define		PC_REL		1

struct t_operand {
		int type, num;
		char *lab, *indx, *cst, *offset;
		/* Changed names of identifiers "index" and "const" */
	};

#define fit_6bits(val)		((unsigned long)(val) < 64)
#define fit_byte(val)		((unsigned long)((val)+128) < 256)
#define fit_word(val)		((unsigned long)((val)+32768L) < 65536L)
