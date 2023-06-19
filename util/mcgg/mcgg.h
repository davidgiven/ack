#ifndef MCGG_H
#define MCGG_H

/* Excruciating macro which packs ir opcodes and sizes into an int for iburg's benefit.
 *
 * Types are mapped to: I=1, F=2, L=3, D=4
 */
#define ir_to_esn(iropcode, type) \
	((iropcode)*5 + \
		(((type) == 'I') ? 1 : \
		 ((type) == 'F') ? 2 : \
		 ((type) == 'L') ? 3 : \
         ((type) == 'D') ? 4 : 0))

#define STATE_TYPE void*

#define STATE_LABEL(p) ((p)->state_label)
#define OP_LABEL(p) ((p)->label)
#define LEFT_CHILD(p) ((p)->left)
#define RIGHT_CHILD(p) ((p)->right)

struct burm_node
{
    int label;
    void* state_label;
    struct burm_node* left;
    struct burm_node* right;
    struct ir* ir;
};

typedef struct burm_node* NODEPTR_TYPE;

extern void* burm_label(NODEPTR_TYPE node);
extern int burm_rule(void* state, int goalnt);
extern const short *burm_nts[];
extern NODEPTR_TYPE* burm_kids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]);
extern void burm_trace(NODEPTR_TYPE p, int ruleno, int cost, int bestcost);

struct burm_emitter_data
{
    void (*emit_string)(const char* data);
    void (*emit_fragment)(int child);
    void (*emit_return_reg)(int index);
    void (*emit_reg)(int child, int index);
    void (*emit_value)(int child);
    void (*emit_eoi)(void);
    void (*constrain_input_reg)(int child, uint32_t attr);
    void (*constrain_input_reg_preserved)(int child);
    void (*constrain_output_reg)(uint32_t attr);
    void (*constrain_output_reg_equal_to)(int child);
};

typedef void burm_emitter_t(const struct burm_emitter_data* data);

struct burm_instruction_data
{
    const char* name;
    burm_emitter_t* emitter;
    bool is_fragment;
    uint32_t corrupts;
};

extern const struct burm_instruction_data burm_instruction_data[];

struct burm_register_data
{
    /* The primary name of this register. */

    const char* id;

    /* The type bitfield of this register (register classes). */

    uint32_t attrs;

    /* Any additional names this register might have (null-terminated). */

    const char** names;

    /* A list of registers that this register is aliased with (null-terminated). */

    const struct burm_register_data** aliases;
};

extern const struct burm_register_data burm_register_data[];
extern const char* burm_register_class_names[];

enum
{
    NONTERM_STMT = 1
};

#endif

/* vim: set sw=4 ts=4 expandtab : */
