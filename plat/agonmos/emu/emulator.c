#define _POSIX_C_SOURCE 199309
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "globals.h"
#include "cemu/core/cpu.h"
#include "zdis/zdis.h"

extern uint8_t ram[RAMSIZE];
bool breakpoints[RAMSIZE];

struct watchpoint
{
	uint16_t address;
	uint8_t value;
	bool enabled;
};

static struct watchpoint watchpoints[16];
static bool tracing = false;
static bool singlestepping = true;
static bool rstbreak = false;
static char disassembler_buffer[64];

static int zdis_read(struct zdis_ctx* ctx, uint32_t address)
{
	return ram[address];
}

uint8_t mem_read_cpu(uint32_t address, bool cpu)
{
	if (address >= RAMSIZE)
		fatal("out of range memory access");
	return ram[address];
}

void mem_write_cpu(uint32_t address, uint8_t value)
{
	if (address >= RAMSIZE)
		fatal("out of range memory access");
	ram[address] = value;
}

uint8_t port_read_byte(uint32_t address)
{
	return 0;
}

void port_write_byte(uint32_t address, uint8_t value)
{
}

static bool
zdis_put(struct zdis_ctx* ctx, enum zdis_put kind, int32_t val, bool il)
{
	char pattern[8];
	char* p = pattern;
	char* wp = disassembler_buffer + strlen(disassembler_buffer);

	switch (kind)
	{
		case ZDIS_PUT_REL: // JR/DJNZ targets
			val += ctx->zdis_end_addr;
			return zdis_put(ctx, ZDIS_PUT_WORD, val, il);

		case ZDIS_PUT_OFF: // immediate offsets from index registers
			if (val > 0)
			{
				sprintf(wp, "+%02x", val);
			}
			else if (val < 0)
			{
				sprintf(wp, "-%02x", -val);
			}
			return true;

		case ZDIS_PUT_BYTE: // byte immediates
		case ZDIS_PUT_PORT: // immediate ports
		case ZDIS_PUT_RST: // RST targets
			sprintf(wp, "%02x", val & 0xff);
			return true;

		case ZDIS_PUT_ABS: // JP/CALL immediate targets
			sprintf(wp, "%06x", val);
			return true;

		case ZDIS_PUT_WORD: // word immediates (il ? 24 : 16) bits wide
		case ZDIS_PUT_ADDR: // load/store immediate addresses
			sprintf(wp, "%06x", val);
			return true;

		case ZDIS_PUT_CHAR: // one character of mnemonic, register, or
		                    // parentheses
			*wp++ = val;
			*wp = 0;
			return true;

		case ZDIS_PUT_ARG_SEP: // between two arguments
			*wp++ = ',';
			/* fall through */
		case ZDIS_PUT_MNE_SEP: // between mnemonic and arguments
			*wp++ = ' ';
			*wp = 0;
			return true;

		case ZDIS_PUT_END: // at end of instruction
			return true;
	}
	// return false for error
	return false;
}

static struct zdis_ctx disctx = {
	.zdis_read = zdis_read, // callback for getting bytes to disassemble
	.zdis_put = zdis_put, // callback for processing disassebly output
	.zdis_start_addr = 0, // starting address of the current instruction
	.zdis_end_addr = 0, // ending address of the current instruction
	.zdis_lowercase
	= true, // automatically convert ZDIS_PUT_CHAR characters to lowercase
	.zdis_implicit
	= true, // omit certain destination arguments as per z80 style assembly
	.zdis_adl = true, // default word width when not overridden by suffix
};

void showregs(void)
{
	printf(
	    "AF:  %04x HL:%06x BC:%06x DE:%06x IX:%06x IY:%06x\n", cpu.registers.AF,
	    cpu.registers.HL, cpu.registers.BC, cpu.registers.DE, cpu.registers.IX,
	    cpu.registers.IY);
	printf(
	    "          hl:%06x bc:%06x de:%06x PC:%06x SP:%06x\n",
	    cpu.registers._HL, cpu.registers._BC, cpu.registers._DE,
	    cpu.registers.PC, cpu.registers.SPL);

	disassembler_buffer[0] = 0;
	disctx.zdis_start_addr = disctx.zdis_end_addr = cpu.registers.PC;
	zdis_put_inst(&disctx);

	printf("%06x : ", cpu.registers.pc);
	int bytes = disctx.zdis_end_addr - disctx.zdis_start_addr;
	for (int i = 0; i < 5; i++)
	{
		if (i < bytes)
			printf("%02x ", ram[cpu.registers.PC + i]);
		else
			printf("   ");
	}
	printf(": %s\n", disassembler_buffer);
}

static void cmd_register(void)
{
	char* w1 = strtok(NULL, " ");
	char* w2 = strtok(NULL, " ");

	if (w1 && w2)
	{
		uint32_t value = strtoul(w2, NULL, 16);

		if (strcmp(w1, "sp") == 0)
			cpu.registers.SPL = value;
		else if (strcmp(w1, "pc") == 0)
			cpu.registers.PC = value;
		else if (strcmp(w1, "a") == 0)
			cpu.registers.A = value;
		else if (strcmp(w1, "af") == 0)
			cpu.registers.AF = value;
		else if (strcmp(w1, "hl") == 0)
			cpu.registers.HL = value;
		else if (strcmp(w1, "bc") == 0)
			cpu.registers.BC = value;
		else if (strcmp(w1, "de") == 0)
			cpu.registers.DE = value;
		else if (strcmp(w1, "ix") == 0)
			cpu.registers.IX = value;
		else if (strcmp(w1, "iy") == 0)
			cpu.registers.IY = value;
		else
		{
			printf("Bad register\n");
			return;
		}
	}

	showregs();
}

static void cmd_break(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint32_t breakpc = strtoul(w1, NULL, 16);
		if (breakpc < RAMSIZE)
			breakpoints[breakpc] = true;
	}
	else
	{
		for (int i = 0; i < RAMSIZE; i++)
		{
			if (breakpoints[i])
				printf("%04x\n", i);
		}
	}
}

static void cmd_delete_breakpoint(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint32_t breakpc = strtoul(w1, NULL, 16);
		if (breakpc < RAMSIZE)
			breakpoints[breakpc] = false;
	}
}

static void cmd_watch(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint16_t watchaddr = strtoul(w1, NULL, 16);
		for (int i = 0; i < sizeof(watchpoints) / sizeof(*watchpoints); i++)
		{
			struct watchpoint* w = &watchpoints[i];
			if (!w->enabled)
			{
				w->address = watchaddr;
				w->enabled = true;
				w->value = ram[watchaddr];
				return;
			}
		}
		printf("Too many breakpoints\n");
	}
	else
	{
		for (int i = 0; i < sizeof(watchpoints) / sizeof(*watchpoints); i++)
		{
			struct watchpoint* w = &watchpoints[i];
			if (w->enabled)
				printf("%04x (current value: %02x)\n", w->address, w->value);
		}
	}
}

static void cmd_delete_watchpoint(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint16_t address = strtoul(w1, NULL, 16);
		for (int i = 0; i < sizeof(breakpoints) / sizeof(*breakpoints); i++)
		{
			struct watchpoint* w = &watchpoints[i];
			if (w->enabled && (w->address == address))
			{
				w->enabled = false;
				return;
			}
		}
		printf("No such watchpoint\n");
	}
}

static void cmd_memory(void)
{
	char* w1 = strtok(NULL, " ");
	char* w2 = strtok(NULL, " ");

	if (!w2)
		w2 = "100";

	if (w1 && w2)
	{
		uint32_t startaddr = strtoul(w1, NULL, 16);
		uint32_t endaddr = startaddr + strtoul(w2, NULL, 16);
		uint32_t startrounded = startaddr & ~0xf;
		uint32_t endrounded = (endaddr + 0xf) & ~0xf;

		uint32_t p = startrounded;
		if (endrounded > RAMSIZE)
			endrounded = RAMSIZE;
		if (startrounded > RAMSIZE)
			return;

		while (p < endrounded)
		{
			printf("%06x : ", p);
			for (int i = 0; i < 16; i++)
			{
				uint16_t pp = p + i;
				if ((pp >= startaddr) && (pp < endaddr))
					printf("%02x ", ram[pp]);
				else
					printf("   ");
			}
			printf(": ");
			for (int i = 0; i < 16; i++)
			{
				uint16_t pp = p + i;
				if ((pp >= startaddr) && (pp < endaddr))
				{
					uint8_t c = ram[pp];
					if ((c < 32) || (c > 127))
						c = '.';
					putchar(c);
				}
				else
					putchar(' ');
			}
			p += 16;
			putchar('\n');
		}
	}
}

static void cmd_unassemble(void)
{
	char* w1 = strtok(NULL, " ");
	char* w2 = strtok(NULL, " ");

	if (!w2)
		w2 = "10";

	if (w1 && w2)
	{
		uint32_t addr = strtoul(w1, NULL, 16);
		uint32_t endaddr = addr + strtoul(w2, NULL, 16);

		while (addr < endaddr)
		{
			disassembler_buffer[0] = 0;
			disctx.zdis_start_addr = disctx.zdis_end_addr = addr;
			zdis_put_inst(&disctx);

			printf("%06x : ", addr);
			int bytes = disctx.zdis_end_addr - disctx.zdis_start_addr;
			for (int i = 0; i < 5; i++)
			{
				if (i < bytes)
					printf("%02x ", ram[addr + i]);
				else
					printf("   ");
			}
			printf(": %s\n", disassembler_buffer);

			addr += bytes;
		}
	}
}

static void cmd_rst(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
		rstbreak = !!strtoul(w1, NULL, 16);
	else
		printf("break on rst: %s\n", rstbreak ? "on" : "off");
}

static void cmd_tracing(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
		tracing = !!strtoul(w1, NULL, 16);
	else
		printf("tracing: %s\n", tracing ? "on" : "off");
}

static void cmd_help(void)
{
	printf("Sleazy debugger\n"
	       "  r               show registers\n"
	       "  r <reg> <value> set register\n"
	       "  b               show breakpoints\n"
	       "  b <addr>        set breakpoint\n"
	       "  db <addr>       delete breakpoint\n"
	       "  w <addr>        set watchpoint\n"
	       "  dw <addr>       delete watchpoint\n"
	       "  m <addr> <len>  show memory\n"
	       "  u <addr> <len>  unassemble memory\n"
	       "  s               single step\n"
	       "  g               continue\n"
	       "  rst 0|1         enable break on rst\n"
	       "  trace 0|1       enable tracing\n");
}

static void debug(void)
{
	bool go = false;
	showregs();
	while (!go)
	{
		char* cmdline = readline("debug>");
		if (!cmdline)
			exit(0);

		char* token = strtok(cmdline, " ");
		if (token != NULL)
		{
			if (strcmp(token, "?") == 0)
				cmd_help();
			else if (strcmp(token, "r") == 0)
				cmd_register();
			else if (strcmp(token, "b") == 0)
				cmd_break();
			else if (strcmp(token, "db") == 0)
				cmd_delete_breakpoint();
			else if (strcmp(token, "w") == 0)
				cmd_watch();
			else if (strcmp(token, "dw") == 0)
				cmd_delete_watchpoint();
			else if (strcmp(token, "m") == 0)
				cmd_memory();
			else if (strcmp(token, "u") == 0)
				cmd_unassemble();
			else if (strcmp(token, "s") == 0)
			{
				singlestepping = true;
				go = true;
			}
			else if (strcmp(token, "g") == 0)
			{
				singlestepping = false;
				go = true;
			}
			else if (strcmp(token, "rst") == 0)
				cmd_rst();
			else if (strcmp(token, "trace") == 0)
				cmd_tracing();
			else
				printf("Bad command\n");
		}

		free(cmdline);
	}
}

static void cpu_rst(uint8_t insn)
{
	switch (insn)
	{
		case 0xc7: /* rst 00 */
			exit(0);

		case 0xd7: /* rst 10 */
			putchar(cpu.registers.A);
			break;

		case 0xdf: /* rst 18 */
		{
			uint32_t addr = cpu.registers.HL;
			uint32_t len = cpu.registers.BC;
			if (len)
			{
				while (len--)
					putchar(mem_read_cpu(addr++, false));
			}
			else
			{
				uint8_t term = cpu.registers.A;
				for (;;)
				{
					char c = mem_read_cpu(addr++, false);
					if (c == term)
						break;
					putchar(c);
				}
			}
			break;
		}

		default:
			fprintf(stderr, "unsupported rst %x\n", insn);
			singlestepping = true;
			return;
	}

	cpu.registers.PC++;
}

static void sigusr1_cb(int number)
{
	singlestepping = true;
}

void emulator_init(void)
{
	memset(ram, 0xee, sizeof(ram));

	cpu_init();
	cpu.ADL = cpu.IL = cpu.L = true;
	singlestepping = flag_enter_debugger;

	struct sigaction action = { .sa_handler = sigusr1_cb };
	sigaction(SIGUSR1, &action, NULL);
}

void emulator_run(void)
{
	for (;;)
	{
		uint32_t pc = cpu.registers.PC;
		singlestepping |= breakpoints[pc];

		for (int i = 0; i < sizeof(watchpoints) / sizeof(*watchpoints); i++)
		{
			struct watchpoint* w = &watchpoints[i];
			if (w->enabled && (ram[w->address] != w->value))
			{
				printf(
				    "\nWatchpoint hit: %x06 has changed from %02x to %02x\n",
				    w->address, w->value, ram[w->address]);
				w->value = ram[w->address];
				singlestepping = true;
			}
		}

		if (singlestepping)
			debug();
		else if (tracing)
			showregs();

		uint8_t insn = ram[pc];
		if ((insn & 0xc7) == 0xc7)
			cpu_rst(insn);
		else
			cpu_execute();
	}
}
