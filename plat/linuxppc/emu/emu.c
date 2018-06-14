#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <strings.h>
#include <byteswap.h>
#include "emu.h"

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

static inline uint32_t ext8(int8_t n)
{
	return (n << 24) >> 24;
}

static inline uint32_t ext16(int16_t n)
{
	return (n << 16) >> 16;
}

static bool getcr(uint8_t bit)
{
	bit = 31 - bit; /* note PowerPC bit numbering */
	return cpu.cr & (1<<bit);
}

static void setcr(uint8_t bit, bool value)
{
	bit = 31 - bit; /* note PowerPC bit numbering */
	cpu.cr = cpu.cr & (1<<bit) | (value<<bit);
}

static void setcr0(bool setcr0, uint32_t value)
{
	if (setcr0)
		fatal("setcr0 not supported yet");
}

static void mcrf(uint8_t destfield, uint8_t srcfield)
{
	fatal("mcrf not supported yet");
}

static void branch(uint8_t bo, uint8_t bi, uint32_t dest, bool a_bit, bool l_bit)
{
	fatal("branch not supported yet");
}

static void read_multiple(uint32_t address, uint8_t reg)
{
	fatal("read_multiple not supported yet");
}

static void write_multiple(uint32_t address, uint8_t reg)
{
	fatal("write_multiple not supported yet");
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
	
	return a / b;
}

static uint32_t divuo(uint32_t a, uint32_t b, bool set_o)
{
	if (set_o)
		fatal("can't use O bit in divu yet");
	
	return a / b;
}

static void compares(int32_t a, int32_t b, uint8_t field)
{
	fatal("compares not supported yet");
}

static void compareu(uint32_t a, uint32_t b, uint8_t field)
{
	fatal("compareu not supported yet");
}

static uint32_t cntlzw(uint32_t source)
{
	return 32 - ffs(source);
}

static uint32_t popcntb(uint32_t source)
{
	fatal("popcntb not supported");
}

static uint32_t rlwnm(uint32_t source, uint8_t shift, uint8_t mb, uint8_t me)
{
	fatal("rlwnm not supported");
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
	fatal("unimplemented instruction 0x%0x", value);
}

static void single_step(void)
{
	uint32_t value = read_long(cpu.cia);
	cpu.nia = cpu.cia + 4;
	dispatch(value);
	cpu.cia = cpu.nia;
}

