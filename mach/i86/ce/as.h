#define	UNKNOWN		0
#define	IS_REG		0x1
#define	IS_ACCU		0x2
#define	IS_DATA		0x4
#define	IS_LABEL	0x8
#define	IS_MEM		0x10
#define	IS_ADDR		0x20
#define	IS_ILB		0x40

#define AX		0
#define BX		3
#define CL		1
#define SP		4
#define BP		5
#define SI		6
#define DI		7

#define REG( op)	( op->type & IS_REG)
#define ACCU( op)	( op->type & IS_REG  &&  op->reg == AX)
#define REG_CL( op)	( op->type & IS_REG  &&  op->reg == CL)
#define DATA( op)	( op->type & IS_DATA)
#define lABEL( op)	( op->type & IS_LABEL)
#define ILB( op)	( op->type & IS_ILB)
#define MEM( op)	( op->type & IS_MEM)
#define ADDR( op)	( op->type & IS_ADDR)
#define EADDR( op)	( op->type & ( IS_ADDR | IS_MEM | IS_REG))
#define CONST1( op)	( op->type & IS_DATA  && strcmp( "1", op->expr) == 0)
#define MOVS( op)	( op->type & IS_LABEL&&strcmp("\"movs\"", op->lab) == 0)
#define IMMEDIATE( op)	( op->type & ( IS_DATA | IS_LABEL))

#define TRUE		1
#define FALSE		0

struct t_operand {
	unsigned type;
	int reg;
	char *expr, *lab, *off;
       };

extern struct t_operand saved_op, *AX_oper;
