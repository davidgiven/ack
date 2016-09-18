#ifndef MCG_H
#define MCG_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "em_arith.h"
#include "em_label.h"
#include "em.h"
#include "em_comp.h"
#include "em_pseu.h"
#include "em_mnem.h"
#include "em_flag.h"
#include "em_ptyp.h"
#include "array.h"
#include "ir.h"

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

struct symbol {
	const char* name;
	int section;
	bool is_defined : 1;
	bool is_exported : 1;
	bool is_proc : 1;
};

enum {
    PARAM_NONE,
    PARAM_VALUE,
    PARAM_LABEL
};

struct basicblock {
    const char* name;
    ARRAY(struct ir, irs);
    ARRAY(struct basicblock, inblocks);
    ARRAY(struct basicblock, outblocks);
    ARRAY(struct ir, outs);
    ARRAY(struct ir, ins);
    bool is_wired : 1;
};

extern void fatal(const char* s, ...);
extern const char* aprintf(const char* fmt, ...);

extern void parse_em(void);

extern void symbol_init(void);
extern bool symbol_exists(const char* name);
extern struct symbol* symbol_get(const char* name);
extern void symbol_declare(const char* name, bool is_exported, bool is_proc);

extern void data_label(const char* name);
extern void data_int(arith data, size_t size, bool is_ro);
extern void data_block(const uint8_t* data, size_t size, bool is_ro);
extern void data_offset(const char* label, arith offset, bool is_ro);
extern void data_bss(arith size, int init);

extern void bb_init(void);
extern struct basicblock* bb_get(const char* name);
extern void bb_alias(struct basicblock* block, const char* name);
extern void bb_wire_outs_to_ins(struct basicblock* outblock, struct basicblock* inblock);

extern void tb_filestart(void);
extern void tb_fileend(void);
extern void tb_ilabel(const char* label);
extern void tb_procstart(const char* label, size_t nlocals);
extern void tb_procend(void);
extern void tb_regvar(arith offset, int size, int type, int priority);

extern void tb_insn_simple(int opcode, int flags);
extern void tb_insn_label(int opcode, int flags, const char* label, arith offset);
extern void tb_insn_value(int opcode, int flags, arith value);

#endif

/* vim: set sw=4 ts=4 expandtab : */
