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
		char *lab, *index, *const, *offset;
	};
