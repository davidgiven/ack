#ifndef IR_H
#define IR_H

enum
{
	IRR_LB = -1,
	IRR_AB = -2,
	IRR_SP = -3,
	IRR_RR = -4,
};

enum
{
	IRS_1,
	IRS_2,
	IRS_4,
	IRS_8
};

struct ir
{
	int id;
	int opcode;
	int size;
	struct ir* left;
	struct ir* right;
	union {
		arith ivalue;
		int rvalue;
		const char* lvalue;
		struct basicblock* bvalue;
	} u;
	bool is_sequence : 1;
};

extern const char* ir_names[];

extern struct ir* new_ir0(int opcode, int size);
extern struct ir* new_ir1(int opcode, int size,
	struct ir* c1);
extern struct ir* new_ir2(int opcode, int size,
	struct ir* c1, struct ir* c2);

extern struct ir* new_labelir(const char* label);
extern struct ir* new_wordir(arith value);
extern struct ir* new_constir(int size, arith value);
extern struct ir* new_bbir(struct basicblock* bb);
extern struct ir* new_anyir(int size);
extern struct ir* new_localir(int offset);

extern void ir_print(char k, const struct ir* ir);

#include "ircodes.h"

#endif

