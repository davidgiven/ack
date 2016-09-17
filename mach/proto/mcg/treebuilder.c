#include "mcg.h"

void tb_filestart(void)
{
}

void tb_fileend(void)
{
}

void tb_symbol(const char* name, bool is_exported, bool is_proc)
{
	printf("; symbol name=%s, exported=%s, is_proc=%s\n",
		name,
		is_exported ? "yes" : "no",
		is_proc ? "yes" : "no");
}

void tb_dlabel(const char* label)
{
	printf("; dlabel name=%s\n", label);
}

void tb_ilabel(const char* label)
{
	printf("; ilabel name=%s\n", label);
}

void tb_data(const uint8_t* data, size_t size, bool is_ro)
{
	printf("; data size=%d ro=%s\n",
		size,
		is_ro ? "yes" : "no");
}

void tb_data_offset(const char* label, arith offset, bool is_ro)
{
	printf("; data label=%s offset=%d ro=%s\n",
		label, offset,
		is_ro ? "yes" : "no");
}

void tb_bss(size_t size, uint8_t init)
{
	printf("; bss size=%d init=0x%x\n",
		size, init);
}

void tb_procstart(const char* label, size_t nlocals)
{
	printf("; proc name=%s nlocals=%d\n", label, nlocals);
}

void tb_procend(void)
{
	printf("; endproc\n");
}

void tb_regvar(arith offset, int size, int type, int priority)
{
	printf("; regvar offset=%d size=%d type=%d priority=%d\n",
		offset, size, type, priority);
}

static void printinsn(int opcode, int flags)
{
	printf("; insn %s %c%c%c%c ",
		em_mnem[opcode - sp_fmnem],
		"/CDNFLGWSZOPBR"[flags & EM_PAR],
		(flags & FLO_C) ? 'c' : '.',
		(flags & FLO_P) ? 'p' : '.',
		(flags & FLO_T) ? 't' : '.');
}

void tb_insn_simple(int opcode, int flags)
{
	printinsn(opcode, flags);
	printf("\n");
}

void tb_insn_label(int opcode, int flags, const char* label, arith offset)
{
	printinsn(opcode, flags);
	printf("label=%s offset=%d\n", label, offset);
}

void tb_insn_value(int opcode, int flags, arith value)
{
	printinsn(opcode, flags);
	printf("value=%d\n", value);
}

/* vim: set sw=4 ts=4 expandtab : */

