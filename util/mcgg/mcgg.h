#ifndef MCGG_H
#define MCGG_H

/* Excruciating macro which packs ir opcodes and sizes into an int for iburg's benefit.
 *
 * Sizes are mapped as: 0=1, 1=1, 2=2, 4=3, 8=4.
 */
#define ir_to_esn(iropcode, size) \
	((iropcode)*4 + \
		(((size) == 4) ? 2 : \
		 ((size) == 8) ? 3 : \
		 ((size) == 0) ? 0 : \
		  (size-1)))


#define STATE_TYPE void*
typedef struct ir* NODEPTR_TYPE;

#define STATE_LABEL(p) ((p)->state_label)

extern void* burm_label(struct ir* ir);
extern int burm_rule(void* state, int goalnt);
extern const short *burm_nts[];
extern struct ir** burm_kids(struct ir* p, int eruleno, struct ir* kids[]);
extern void burm_trace(struct ir* p, int ruleno, int cost, int bestcost);

struct burm_emitter_data
{
    void (*emit_string)(const char* data);
    void (*emit_fragment)(struct ir* ir, int goal);
    void (*emit_reg)(struct ir* ir, int goal);
    void (*emit_value)(struct ir* ir);
    void (*emit_resultreg)(void);
    void (*emit_eoi)(void);
    void (*emit_usereg)(struct ir* ir);
};

typedef void burm_emitter_t(struct ir* ir, const struct burm_emitter_data* data);

struct burm_instruction_data
{
    const char* name;
    burm_emitter_t* emitter;
    int allocate;
    bool is_fragment;
};

extern const struct burm_instruction_data burm_instruction_data[];
extern const char* burm_register_class_names[];

#endif

/* vim: set sw=4 ts=4 expandtab : */
