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

extern char em_pseu[][4];
extern char em_mnem[][4];
extern char em_flag[];


extern void fatal(const char* s, ...);

extern void parse_em(void);

extern void tb_filestart(void);
extern void tb_fileend(void);
extern void tb_symbol(const char* name, bool is_exported, bool is_proc);
extern void tb_dlabel(const char* label);
extern void tb_ilabel(const char* label);
extern void tb_data(const uint8_t* data, size_t size, bool is_ro);
extern void tb_data_offset(const char* label, arith offset, bool is_ro);
extern void tb_bss(size_t size, uint8_t init);
extern void tb_procstart(const char* label, size_t nlocals);
extern void tb_procend(void);
extern void tb_regvar(arith offset, int size, int type, int priority);

extern void tb_insn_simple(int opcode, int flags);
extern void tb_insn_label(int opcode, int flags, const char* label, arith offset);
extern void tb_insn_value(int opcode, int flags, arith value);

#endif

/* vim: set sw=4 ts=4 expandtab : */
