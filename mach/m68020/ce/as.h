#define ARITH_FMT	"%ld"
#define INT_FMT		"%d"

struct t_operand {
	unsigned type;
	char *expr, *lbl;
	int val, reg, reg2, scale, expr2;
       };

#define IS_QUICK		1
#define IS_IMMEDIATE		2
#define IS_A_REG		3
#define IS_D_REG		4
#define IS_REG_PAIR		5
#define IS_INCR			6
#define IS_DECR			7
#define IS_3_OPS		8
#define IS_IND_MEM		9
#define IS_IND_IND		10
#define IS_GLOB_LBL		11
#define IS_LOC_LBL		12
#define IS_IND_REG		13
#define IS_IND_REG_DISPL	14
#define IS_IND_REG_EDISPL	15

#define QUICK( op)	(op->type == IS_QUICK)
#define IMMEDIATE( op)	(op->type == IS_IMMEDIATE)
#define A_REG( op)	(op->type == IS_A_REG)
#define D_REG( op)	(op->type == IS_D_REG)
#define REG_PAIR( op)	(op->type == IS_REG_PAIR)
#define INCR( op)	(op->type == IS_INCR)
#define REG( op)	(op->type == IS_A_REG || op->type == IS_D_REG)
#define LOC_LBL( op)	(op->type == IS_LOC_LBL)
#define GLOB_LBL( op)	(op->type == IS_GLOB_LBL)
