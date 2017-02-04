#ifndef MCGG_H
#define MCGG_H

#include "mcgg_common.h"

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
    void (*constrain_input_reg_corrupted)(int child);
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
    const char* id;
    uint32_t attrs;
    const char** names;
    burm_register_bitmap_t bitmap;
};

struct burm_regclass_data
{
    const char* name;
    const struct burm_register_data** registers;
    burm_register_bitmap_t bitmap;
    int weight;
};

extern const struct burm_register_data burm_register_data[];
extern const struct burm_regclass_data burm_regclass_data[];

#endif

/* vim: set sw=4 ts=4 expandtab : */
