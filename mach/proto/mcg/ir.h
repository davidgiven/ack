#ifndef IR_H
#define IR_H

#include "ircodes.h"

struct value
{
	struct ir* ir;
	int subid;
	uint32_t attrs;
};

extern uint32_t value_hash_function(void* key);
extern bool value_comparison_function(void* key1, void* key2);
#define HASHTABLE_OF_VALUES \
	{ value_hash_function, value_comparison_function }
extern const struct hashtable empty_hashtable_of_values;

struct ir
{
	/* A struct ir must be castable to a struct value */
	struct value value;

	int id;
	enum ir_opcode opcode;
	int size;
    char type;
	struct ir* left;
	struct ir* right;
	struct ir* root;
	struct basicblock* bb;
	union
	{
		arith ivalue;
		int rvalue;
		const char* lvalue;
		struct basicblock* bvalue;
        PMAPOF(struct basicblock, struct ir) phivalue;
	} u;
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

typedef bool ir_walker_t(struct ir* node, void* user);
extern struct ir* ir_walk(struct ir* ir, ir_walker_t* callback, void* user);
extern struct ir* ir_find(struct ir* ir, int opcode);

extern void ir_print(char k, const struct ir* ir);

extern void ir_rewrite_single_block(struct basicblock* bb, struct ir* src, struct ir* dest);
extern void ir_rewrite(struct basicblock* bb, struct ir* src, struct ir* dest);

#endif

/* vim: set sw=4 ts=4 expandtab : */
