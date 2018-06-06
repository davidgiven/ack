#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "sim.h"
#include "m68k.h"

void disassemble_program();

#define ADDRESS_MASK 0xffffffff
#define RAM_BASE 0x08000000
#define RAM_TOP  0x08100000

#define INIT_SP RAM_TOP
#define INIT_PC 0x08000054

/* Read/write macros */
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) |			\
							  (BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) |			\
							  ((BASE)[(ADDR)+1]<<16) |		\
							  ((BASE)[(ADDR)+2]<<8) |		\
							  (BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL)&0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff;		\
									(BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff;		\
									(BASE)[(ADDR)+1] = ((VAL)>>16)&0xff;	\
									(BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;		\
									(BASE)[(ADDR)+3] = (VAL)&0xff


static void exit_error(char* fmt, ...);
static void syscall(void);

unsigned int cpu_read_byte(unsigned int address);
unsigned int cpu_read_word(unsigned int address);
unsigned int cpu_read_long(unsigned int address);
void cpu_write_byte(unsigned int address, unsigned int value);
void cpu_write_word(unsigned int address, unsigned int value);
void cpu_write_long(unsigned int address, unsigned int value);

unsigned char g_ram[RAM_TOP - RAM_BASE];


/* Exit with an error message.  Use printf syntax. */
void exit_error(char* fmt, ...)
{
	static int guard_val = 0;
	char buff[100];
	unsigned int pc;
	va_list args;

	if(guard_val)
		return;
	else
		guard_val = 1;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	pc = m68k_get_reg(NULL, M68K_REG_PPC);
	m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
	fprintf(stderr, "At %04x: %s\n", pc, buff);

	exit(EXIT_FAILURE);
}

static inline unsigned int transform_address(unsigned int address)
{
	unsigned int i = (address & ADDRESS_MASK) - RAM_BASE;
	if (i >= (unsigned int)(RAM_TOP - RAM_BASE))
		exit_error("Attempted to read from RAM address %08x %08x", address, i);
	return i;
}

unsigned int cpu_read_long(unsigned int address)
{
	switch (address)
	{
		case 0x00: return INIT_SP;
		case 0x04: return INIT_PC;
		case 0x80: syscall(); return 0x10000;
		case 0x10000: return 0x4e734e73; /* rte; rte */
		case 0x10004: return 0;
		default: return READ_LONG(g_ram, transform_address(address));
	}
}


unsigned int cpu_read_word(unsigned int address)
{
	unsigned int l = cpu_read_long(address & ~3);
	l >>= 16 - (address & 2)*8;
	return l & 0xffff;
}

unsigned int cpu_read_byte(unsigned int address)
{
	unsigned int l = cpu_read_long(address & ~3);
	l >>= 24 - (address & 3)*8;
	return l & 0xff;
}

unsigned int cpu_read_word_dasm(unsigned int address)
{
	return cpu_read_word(address);
}

unsigned int cpu_read_long_dasm(unsigned int address)
{
	return cpu_read_long(address);
}


/* Write data to RAM or a device */
void cpu_write_byte(unsigned int address, unsigned int value)
{
	WRITE_BYTE(g_ram, transform_address(address), value);
}

void cpu_write_word(unsigned int address, unsigned int value)
{
	WRITE_WORD(g_ram, transform_address(address), value);
}

void cpu_write_long(unsigned int address, unsigned int value)
{
	WRITE_LONG(g_ram, transform_address(address), value);
}

/* Disassembler */
void make_hex(char* buff, unsigned int pc, unsigned int length)
{
	char* ptr = buff;

	for(;length>0;length -= 2)
	{
		sprintf(ptr, "%04x", cpu_read_word_dasm(pc));
		pc += 2;
		ptr += 4;
		if(length > 2)
			*ptr++ = ' ';
	}
}

void disassemble_program()
{
	unsigned int pc;
	unsigned int instr_size;
	char buff[100];
	char buff2[100];

	pc = cpu_read_long_dasm(4);

	while(pc <= 0x16e)
	{
		instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
		make_hex(buff2, pc, instr_size);
		printf("%03x: %-20s: %s\n", pc, buff2, buff);
		pc += instr_size;
	}
	fflush(stdout);
}

void cpu_instr_callback()
{
	/* The following code would print out instructions as they are executed */

#if 1
	static char buff[100];
	static char buff2[100];
	static unsigned int pc;
	static unsigned int instr_size;

	pc = m68k_get_reg(NULL, M68K_REG_PC);
	instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68020);
	make_hex(buff2, pc, instr_size);
	printf("E %03x: %-20s: %s\n", pc, buff2, buff);
	fflush(stdout);
#endif
}

static void syscall(void)
{
	int s = m68k_get_reg(NULL, M68K_REG_D0);
	switch (s)
	{
		case 45: /* brk */
			m68k_set_reg(M68K_REG_D0, RAM_TOP-0x1000);
			break;

		default:
			exit_error("unknown system call %d", s);
	}
}

/* The main loop */
int main(int argc, char* argv[])
{
	FILE* fhandle;

	if(argc != 2)
	{
		printf("Usage: sim <program file>\n");
		exit(-1);
	}

	if((fhandle = fopen(argv[1], "rb")) == NULL)
		exit_error("Unable to open %s", argv[1]);

	if(fread(g_ram, 1, RAM_TOP - RAM_BASE, fhandle) <= 0)
		exit_error("Error reading %s", argv[1]);

//	disassemble_program();

	m68k_init();
	m68k_set_cpu_type(M68K_CPU_TYPE_68020);
	m68k_pulse_reset();

	/* On entry, the Linux stack looks like this.
	 * 
	 * sp+..           NULL
     * sp+8+(4*argc)   env (X quads)
     * sp+4+(4*argc)   NULL
     * sp+4            argv (argc quads)
     * sp              argc
	 *
	 * We'll set it up with a bodgy stack frame with argc=0 just to keep the
	 * startup code happy.
	 */

	{
		unsigned int sp = INIT_SP;
		cpu_write_long(sp -= 4, 0);
		unsigned int envp = sp;
		cpu_write_long(sp -= 4, envp);
		cpu_write_long(sp -= 4, 0);
		unsigned long argv = sp;
		cpu_write_long(sp -= 4, argv);
		cpu_write_long(sp -= 4, 0);
		m68k_set_reg(M68K_REG_SP, sp);
	}

	for (;;)
		m68k_execute(100000);

	return 0;
}

