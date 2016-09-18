#ifndef IR_H
#define IR_H

#include "ircodes.h"

enum
{
	IRR_LB = -1,
	IRR_AB = -2,
	IRR_SP = -3,
	IRR_RR = -4,
};

struct ir
{
	int id;
	int opcode;
	int size;
	struct ir* children[3];
	union {
		arith ivalue;
		int rvalue;
		const char* lvalue;
		struct basicblock* bvalue;
		struct {
			ARRAY(struct ir, srcs);
		} phivalue;
	} u;
	bool sequence : 1;
	bool terminates : 1;
};

extern const char* ir_names[];

extern struct ir* new_ir0(int opcode, int size);
extern struct ir* new_ir1(int opcode, int size,
	struct ir* c1);
extern struct ir* new_ir2(int opcode, int size,
	struct ir* c1, struct ir* c2);
extern struct ir* new_ir3(int opcode, int size,
	struct ir* c1, struct ir* c2, struct ir* c3);

extern struct ir* new_labelir(const char* label);
extern struct ir* new_regir(int reg);
extern struct ir* new_wordir(arith value);
extern struct ir* new_bbir(struct basicblock* bb);
extern struct ir* new_anyir(int size);
extern struct ir* new_phiir(int size);

extern void ir_print(const struct ir* ir);

#endif

