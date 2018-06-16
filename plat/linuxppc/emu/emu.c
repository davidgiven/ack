#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <strings.h>
#include <byteswap.h>
#include "emu.h"

#define BO4 (1<<0)
#define BO3 (1<<1)
#define BO2 (1<<2)
#define BO1 (1<<3)
#define BO0 (1<<4)

cpu_t cpu;

static inline bool carry(void)
{
	fatal("carry() not supported yet");
}

#define swb16(x) bswap_16(x)
#define swb32(x) bswap_32(x)

static inline uint32_t reg(uint8_t n)
{
	return cpu.gpr[n];
}

static inline uint32_t reg0(uint8_t n)
{
	if (n == 0)
		return 0;
	return cpu.gpr[n];
}

static inline uint64_t tobytes(double n)
{
	return *(uint64_t*)&n;
}

static inline double fpr(uint8_t n)
{
	return *(double*)&cpu.fpr[n];
}

static inline uint32_t ext8(int8_t n)
{
	return (n << 24) >> 24;
}

static inline uint32_t ext16(int16_t n)
{
	return (n << 16) >> 16;
}

static inline uint32_t ext26(int32_t n)
{
	return (n << 6) >> 6;
}

static bool getcr(uint8_t bit)
{
	bit = 31 - bit; /* note PowerPC bit numbering */
	return cpu.cr & (1<<bit);
}

static void setcr(uint8_t bit, bool value)
{
	bit = 31 - bit; /* note PowerPC bit numbering */
	cpu.cr = cpu.cr & ~(1<<bit) | (value<<bit);
}

static void setcr0(bool setcr0, uint32_t value)
{
	if (setcr0)
	{
		setcr(0, (int32_t)value < 0);
		setcr(1, (int32_t)value > 0);
		setcr(2, value == 0);
		setcr(3, cpu.xer & (1<<31));
	}
}

static void setcr1(bool setcr1, uint64_t value)
{
	if (setcr1)
		fatal("setcr1 not implemented yet");
}

static void mcrf(uint8_t destfield, uint8_t srcfield)
{
	fatal("mcrf not supported yet");
}

static void branch(uint8_t bo, uint8_t bi, uint32_t dest, bool a_bit, bool l_bit)
{
	bool bo0 = bo & BO0;
	bool bo1 = bo & BO1;
	bool bo2 = bo & BO2;
	bool bo3 = bo & BO3;
	bool ctr_ok;
	bool cond_ok;

	if (!bo2)
		cpu.ctr--;
	ctr_ok = bo2 || (!!cpu.ctr ^ bo3);
	cond_ok = bo0 || (!!(cpu.cr & (1<<(31-bi))) == bo1);
	if (ctr_ok && cond_ok)
	{
		if (a_bit)
			cpu.nia = dest;
		else
			cpu.nia = dest + cpu.cia;
	}
	if (l_bit)
		cpu.lr = cpu.cia + 4;
}

static void read_multiple(uint32_t address, uint8_t reg)
{
	while (reg != 32)
	{
		cpu.gpr[reg] = read_long(address);
		reg++;
		address += 4;
	}
}

static void write_multiple(uint32_t address, uint8_t reg)
{
	while (reg != 32)
	{
		write_long(address, cpu.gpr[reg]);
		reg++;
		address += 4;
	}
}

static void read_string(uint32_t address, uint8_t reg, uint8_t bytes)
{
	fatal("read_string not supported yet");
}

static void write_string(uint32_t address, uint8_t reg, uint8_t bytes)
{
	fatal("write_string not supported yet");
}

static uint32_t addo(uint32_t a, uint32_t b, uint32_t c, bool set_o, bool set_c)
{
	if (set_o || set_c)
		fatal("can't use O or C bits in add yet");
	
	return a + b + c;
}

static uint32_t mulo(uint32_t a, uint32_t b, bool set_o)
{
	if (set_o)
		fatal("can't use O bit in mul yet");
	
	return a * b;
}

static int32_t divo(int32_t a, int32_t b, bool set_o)
{
	if (set_o)
		fatal("can't use O bit in div yet");
	
	if (b == 0)
		return 0;
	return a / b;
}

static uint32_t divuo(uint32_t a, uint32_t b, bool set_o)
{
	if (set_o)
		fatal("can't use O bit in divu yet");
	
	if (b == 0)
		return 0;
	return a / b;
}

static void compares(int32_t a, int32_t b, uint8_t field)
{
	uint8_t bit = field*4;
	setcr(bit+0, a<b);
	setcr(bit+1, a>b);
	setcr(bit+2, a==b);
	setcr(bit+3, cpu.xer & (1<<31));
}

static void compareu(uint32_t a, uint32_t b, uint8_t field)
{
	uint8_t bit = field*4;
	setcr(bit+0, a<b);
	setcr(bit+1, a>b);
	setcr(bit+2, a==b);
	setcr(bit+3, cpu.xer & (1<<31));
}

static uint32_t cntlzw(uint32_t source)
{
	return 32 - ffs(source);
}

static uint32_t popcntb(uint32_t source)
{
	fatal("popcntb not supported");
}

static uint32_t rotate(uint32_t i, uint32_t shift)
{
	return (i << shift) | (i >> (32-shift));
}

static uint32_t rlwnm(uint32_t source, uint8_t shift, uint8_t mb, uint8_t me)
{
	uint8_t masksize = 1 + me - mb; /* me and mb are inclusive */
	uint32_t mask = ((1<<masksize)-1) << (31 - me);
	return rotate(source, shift) & mask;
}

static uint32_t rlwmi(uint32_t source, uint8_t shift, uint8_t mb, uint8_t me)
{
	fatal("rlwmi not supported");
}

static void mtcrf(uint8_t fxm, uint32_t value)
{
	fatal("mtcrf not supported");
}

static void dispatch(uint32_t value)
{
	#include "dispatcher.h"
	fatal("unimplemented instruction 0x%0x (major opcode %d)", value, value>>26);
}

void dump_state(FILE* stream)
{
	int i;

	fprintf(stream, "\n");
	fprintf(stream, "pc=0x%08x lr=0x%08x ctr=0x%08x xer=0x%08x cr=0x%08x\n",
		cpu.cia, cpu.lr, cpu.ctr, cpu.xer, cpu.cr);
	for (i=0; i<32; i++)
	{
		if ((i % 4) == 0)
			fprintf(stream, "\n");
		fprintf(stream, "gpr%02d=0x%08x ", i, cpu.gpr[i]);
	}
	fprintf(stream, "\n");

	/* This might fail and cause a reentrant trap if cia is invalid, so
	 * do it last. */
	fprintf(stream, "insn=0x%08x", read_long(cpu.cia));
}

void single_step(void)
{
	uint32_t value = read_long(cpu.cia);
	cpu.nia = cpu.cia + 4;
	dispatch(value);
	cpu.cia = cpu.nia;
}

