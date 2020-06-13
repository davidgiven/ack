#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>

#include "sim.h"
#include "m68k.h"

void disassemble_program();

#define ADDRESS_MASK 0xffffffff
#define RAM_BASE 0x08000000
#define RAM_TOP  0x08100000

#define BRK_TOP (RAM_TOP - 0x1000)

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
static void emulated_syscall(void);

uint32_t cpu_read_byte(uint32_t address);
uint32_t cpu_read_word(uint32_t address);
uint32_t cpu_read_long(uint32_t address);
void cpu_write_byte(uint32_t address, uint32_t value);
void cpu_write_word(uint32_t address, uint32_t value);
void cpu_write_long(uint32_t address, uint32_t value);

unsigned char g_ram[RAM_TOP - RAM_BASE];
uint32_t brkbase = RAM_BASE;
uint32_t brkpos = RAM_BASE;
uint32_t entrypoint = RAM_BASE;

/* Exit with an error message.  Use printf syntax. */
void exit_error(char* fmt, ...)
{
	static int guard_val = 0;
	char buff[100];
	uint32_t pc;
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
	m68k_disassemble(buff, pc, M68K_CPU_TYPE_68020);
	fprintf(stderr, "At %04x: %s\n", pc, buff);

	exit(EXIT_FAILURE);
}

static inline uint32_t transform_address(uint32_t address)
{
	uint32_t i = (address & ADDRESS_MASK) - RAM_BASE;
	if (i >= (uint32_t)(RAM_TOP - RAM_BASE))
		exit_error("Attempted to read from RAM address %08x", address);
	return i;
}

uint32_t cpu_read_long(uint32_t address)
{
	switch (address)
	{
		case 0x00: return INIT_SP;
		case 0x04: return entrypoint;
		case 0x80: emulated_syscall(); return 0x10000;
		case 0x10000: return 0x4e734e73; /* rte; rte */
		case 0x10004: return 0;
		default:
		{
			uint32_t value = READ_LONG(g_ram, transform_address(address));
			#if 0
			printf("read %08x from %08x\n", value, address);
			#endif
			return value;
		}
	}
}


uint32_t cpu_read_word(uint32_t address)
{
	uint32_t l = cpu_read_long(address & ~3);
	l >>= 16 - (address & 2)*8;
	return l & 0xffff;
}

uint32_t cpu_read_byte(uint32_t address)
{
	uint32_t l = cpu_read_long(address & ~3);
	l >>= 24 - (address & 3)*8;
	return l & 0xff;
}

uint32_t cpu_read_word_dasm(uint32_t address)
{
	return cpu_read_word(address);
}

uint32_t cpu_read_long_dasm(uint32_t address)
{
	return cpu_read_long(address);
}


/* Write data to RAM or a device */
void cpu_write_byte(uint32_t address, uint32_t value)
{
	WRITE_BYTE(g_ram, transform_address(address), value);
}

void cpu_write_word(uint32_t address, uint32_t value)
{
	WRITE_WORD(g_ram, transform_address(address), value);
}

void cpu_write_long(uint32_t address, uint32_t value)
{
	WRITE_LONG(g_ram, transform_address(address), value);
}

/* Disassembler */
void make_hex(char* buff, uint32_t pc, uint32_t length)
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
	uint32_t pc;
	uint32_t instr_size;
	char buff[100];
	char buff2[100];

	pc = cpu_read_long_dasm(4);
	printf("entry point is %0x\n", entrypoint);
	printf("pc is %0x\n", pc);


	while(pc <= entrypoint + 0x16e)
	{
		instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68020);
		make_hex(buff2, pc, instr_size);
		printf("%03x: %-20s: %s\n", pc, buff2, buff);
		pc += instr_size;
	}
	fflush(stdout);
}

void cpu_instr_callback(int apc)
{
        (void)apc;
	uint32_t pc = m68k_get_reg(NULL, M68K_REG_PC);
	if (pc == 0xc)
		exit_error("address exception");

	/* The following code would print out instructions as they are executed */

#if 0
	static char buff[100];
	static char buff2[100];
	static uint32_t instr_size;

	instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68020);
	make_hex(buff2, pc, instr_size);
	printf("E %03x: %-20s: %s\n", pc, buff2, buff);
	printf("  d0: %08x d1: %08x d2: %08x d3: %08x d4: %08x d5: %08x d6: %08x d7: %08x\n",
		m68k_get_reg(NULL, M68K_REG_D0),
		m68k_get_reg(NULL, M68K_REG_D1),
		m68k_get_reg(NULL, M68K_REG_D2),
		m68k_get_reg(NULL, M68K_REG_D3),
		m68k_get_reg(NULL, M68K_REG_D4),
		m68k_get_reg(NULL, M68K_REG_D5),
		m68k_get_reg(NULL, M68K_REG_D6),
		m68k_get_reg(NULL, M68K_REG_D7));
	printf("  a0: %08x a1: %08x a2: %08x a3: %08x a4: %08x a5: %08x a6: %08x a7: %08x\n",
		m68k_get_reg(NULL, M68K_REG_A0),
		m68k_get_reg(NULL, M68K_REG_A1),
		m68k_get_reg(NULL, M68K_REG_A2),
		m68k_get_reg(NULL, M68K_REG_A3),
		m68k_get_reg(NULL, M68K_REG_A4),
		m68k_get_reg(NULL, M68K_REG_A5),
		m68k_get_reg(NULL, M68K_REG_A6),
		m68k_get_reg(NULL, M68K_REG_A7));
	fflush(stdout);
#endif
}

/**
 * translate simulated linux syscall to native call on host
 * see https://www.lurklurk.org/syscalls.html for m68k syscall numbers
 **/
static void emulated_syscall(void)
{
	int s = m68k_get_reg(NULL, M68K_REG_D0);
	switch (s)
	{
		case 1: /* exit */
			exit(m68k_get_reg(NULL, M68K_REG_D1));

		case 3: /* read */
		{
			uint32_t fd = m68k_get_reg(NULL, M68K_REG_D1);
			uint32_t ptr = m68k_get_reg(NULL, M68K_REG_D2);
			uint32_t count = m68k_get_reg(NULL, M68K_REG_D3);
			m68k_set_reg(M68K_REG_D0, read(fd, g_ram + transform_address(ptr), count));
			break;
		}

		case 4: /* write */
		{
			uint32_t fd = m68k_get_reg(NULL, M68K_REG_D1);
			uint32_t ptr = m68k_get_reg(NULL, M68K_REG_D2);
			uint32_t len = m68k_get_reg(NULL, M68K_REG_D3);
			m68k_set_reg(M68K_REG_D0, write(fd, g_ram + transform_address(ptr), len));
			break;
		}

		case 5: /* open */
		{
			uint32_t pathname = m68k_get_reg(NULL, M68K_REG_D1);
			uint32_t flags = m68k_get_reg(NULL, M68K_REG_D2);
			uint32_t mode = m68k_get_reg(NULL, M68K_REG_D3);
			m68k_set_reg(M68K_REG_D0, open(g_ram + transform_address(pathname), flags, mode));
			break;
		}

		case 6: /* close */
		{
			uint32_t fd = m68k_get_reg(NULL, M68K_REG_D1);
			m68k_set_reg(M68K_REG_D0, close(fd));
			break;
		}

		case 10: /* unlink */
		{
			uint32_t pathname = m68k_get_reg(NULL, M68K_REG_D1);
			m68k_set_reg(M68K_REG_D0, unlink(g_ram + transform_address(pathname)));
			break;
		}

		case 19: /* lseek */
		{
			uint32_t fd = m68k_get_reg(NULL, M68K_REG_D1);
			uint32_t offset = m68k_get_reg(NULL, M68K_REG_D2);
			uint32_t whence = m68k_get_reg(NULL, M68K_REG_D3);
			m68k_set_reg(M68K_REG_D0, lseek(fd, offset, whence));
			break;
		}

		case 45: /* brk */
		{
			uint32_t newpos = m68k_get_reg(NULL, M68K_REG_D1);
			if (newpos == 0)
				m68k_set_reg(M68K_REG_D0, brkpos);
			else if ((newpos < brkbase) || (newpos >= BRK_TOP))
				m68k_set_reg(M68K_REG_D0, -ENOMEM);
			else
			{
				brkpos = newpos;
				m68k_set_reg(M68K_REG_D0, 0);
			}
			break;
		}

		case 20: /* getpid */
		case 48: /* signal */
		case 54: /* ioctl */
		case 78: /* gettimeofday */
			m68k_set_reg(M68K_REG_D0, 0);
			break;

		default:
			exit_error("unknown system call %d", s);
	}
}

static void load_program(FILE* fd)
{
	fseek(fd, 0, SEEK_SET);
	if (fread(g_ram, 1, 0x34, fd) != 0x34)
		exit_error("couldn't read ELF header");
	
	uint32_t phoff = READ_LONG(g_ram, 0x1c);
	uint16_t phentsize = READ_WORD(g_ram, 0x2a);
	uint16_t phnum = READ_WORD(g_ram, 0x2c);
	entrypoint = READ_LONG(g_ram, 0x18);
	if ((phentsize != 0x20) || (phnum != 1))
		exit_error("unsupported ELF file");

	fseek(fd, phoff, SEEK_SET);
	if (fread(g_ram, 1, phentsize, fd) != phentsize)
		exit_error("couldn't read program header");

	uint32_t offset = READ_LONG(g_ram, 0x04);
	uint32_t vaddr = READ_LONG(g_ram, 0x08);
	uint32_t filesz = READ_LONG(g_ram, 0x10);
	uint32_t memsz = READ_LONG(g_ram, 0x14);
	brkbase = brkpos = vaddr + memsz;

	uint32_t vaddroffset = transform_address(vaddr);
	transform_address(vaddr + memsz); /* bounds check */
	memset(g_ram+vaddroffset, 0, memsz);
	fseek(fd, offset, SEEK_SET);
	if (fread(g_ram+vaddroffset, 1, filesz, fd) != filesz)
		exit_error("couldn't read program data");
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

	load_program(fhandle);

	//disassemble_program();

	m68k_set_cpu_type(M68K_CPU_TYPE_68040);
	m68k_init();
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
		uint32_t sp = INIT_SP;
		cpu_write_long(sp -= 4, 0);
		uint32_t envp = sp;
		cpu_write_long(sp -= 4, envp);
		cpu_write_long(sp -= 4, 0);
		unsigned long argv = sp;
		cpu_write_long(sp -= 4, argv);
		cpu_write_long(sp -= 4, 0);
		m68k_set_reg(M68K_REG_SP, sp);   /* init sp is also addr 0 */
	}

	for (;;) {
		m68k_execute(100000);
	}

	return 0;
}

