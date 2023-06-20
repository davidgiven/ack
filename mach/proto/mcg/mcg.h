#ifndef MCG_H
#define MCG_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "system.h"
#include "em_arith.h"
#include "em_label.h"
#include "em.h"
#include "em_comp.h"
#include "em_pseu.h"
#include "em_mnem.h"
#include "em_flag.h"
#include "em_ptyp.h"
#include "array.h"
#include "imap.h"
#include "pmap.h"
#include "pset.h"
#include "diagnostics.h"
#include "astring.h"
#include "ir.h"
#include "mcgg.h"
#include "reg.h"
#include "hop.h"
#include "basicblock.h"
#include "procedure.h"
#include "graph.h"
#include "tables.h"

extern char em_pseu[][4];
extern char em_mnem[][4];
extern char em_flag[];

enum {
	SECTION_UNKNOWN = 0,
	SECTION_ROM,
	SECTION_DATA,
	SECTION_BSS,
	SECTION_TEXT
};

struct symbol
{
	const char* name;
	int section;
    struct procedure* proc;
	bool is_defined : 1;
	bool is_exported : 1;
	bool is_proc : 1;
};

enum
{
    PARAM_NONE,
    PARAM_IVALUE,
    PARAM_LVALUE,
    PARAM_BVALUE,
};

struct em
{
    int opcode;
    int paramtype;
    union {
        arith ivalue;
        struct {
            const char* label;
            arith offset;
        } lvalue;
        struct {
            struct basicblock* left;
            struct basicblock* right;
        } bvalue;
    } u;
};
    
extern void tracef(char k, const char* fmt, ...);
extern bool tracing(char k);

extern void parse_em(void);

extern void symbol_init(void);
extern bool symbol_exists(const char* name);
extern struct symbol* symbol_get(const char* name);
extern void symbol_declare(const char* name, bool is_exported, bool is_proc);

typedef bool symbol_walker_t(struct symbol* symbol, void* user);
extern struct symbol* symbol_walk(symbol_walker_t* walker, void* user);

extern void data_label(const char* name);
extern void data_int(arith data, size_t size, bool is_ro);
extern void data_float(const char* data, size_t size, bool is_ro);
extern void data_block(const uint8_t* data, size_t size, bool is_ro);
extern void data_offset(const char* label, arith offset, bool is_ro);
extern void data_bss(arith size, int init);

extern void tb_filestart(void);
extern void tb_fileend(void);
extern void tb_procedure(void);
extern void tb_regvar(struct procedure* proc, arith offset, int size, int type, int priority);

extern void pass_assign_hop_producer_consumers(void);
extern void pass_convert_locals_to_ssa(void);
extern void pass_convert_stack_ops(void);
extern void pass_eliminate_trivial_blocks(void);
extern void pass_find_congruence_groups(void);
extern void pass_group_irs(void);
extern void pass_infer_types(void);
extern void pass_insert_moves(void);
extern void pass_instruction_selector(void);
extern void pass_live_vreg_analysis(void);
extern void pass_lower_pushes(void);
extern void pass_add_prologue_epilogue(void);
extern void pass_register_allocator(void);
extern void pass_remove_dead_blocks(void);
extern void pass_remove_dead_phis(void);
extern void pass_split_critical_edges(void);
extern void pass_wire_up_return_values(void);

extern void platform_calculate_offsets(void);
extern struct hop* platform_prologue(void);
extern struct hop* platform_epilogue(void);
extern struct hop* platform_load(struct basicblock* bb, struct move* move);
extern struct hop* platform_store(struct basicblock* bb, struct move* move);
extern const char* platform_label(const char* label);

extern FILE* outputfile;
extern FILE* dominance_dot_file;
extern FILE* cfg_dot_file;

#endif

/* vim: set sw=4 ts=4 expandtab : */
