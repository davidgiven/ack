#define _POSIX_C_SOURCE 199309
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include "intel_8080_emulator.h"
#include "dis8080.h"
#include "globals.h"

uint8_t ram[0x10000];

struct watchpoint
{
	uint16_t address;
	uint8_t value;
	bool enabled;
};

static uint16_t breakpoints[16];
static struct watchpoint watchpoints[16];
static bool tracing = false;
static bool singlestepping = true;
static bool bdosbreak = false;

uint8_t i8080_read(uint16_t addr)
{
	return ram[addr];
}

void i8080_write(uint16_t addr, uint8_t value)
{
	ram[addr] = value;
}

uint8_t i8080_inport(uint8_t addr)
{
	return 0;
}

void i8080_outport(uint8_t addr, uint8_t value)
{
	biosbdos_entry(addr & 0xff);
	if (bdosbreak)
		singlestepping = true;
}

void showregs(void)
{
	uint16_t af = i8080_read_reg16(AF);
	printf("%c%c.%c.%c%c%c sp=%04x af=%04x bc=%04x de=%04x hl=%04x\n",
		(af & 0x80) ? 'S' : 's',
		(af & 0x40) ? 'Z' : 'z',
		(af & 0x10) ? 'H' : 'h',
		(af & 0x04) ? 'P' : 'p',
		(af & 0x02) ? 'N' : 'n',
		(af & 0x01) ? 'C' : 'c',
		i8080_read_reg16(SP),
		af,
		i8080_read_reg16(BC),
		i8080_read_reg16(DE),
		i8080_read_reg16(HL));

	char buffer[80];
	int tstates;
	uint16_t pc = i8080_read_reg16(PC);
	i8080_disassemble(buffer, sizeof(buffer), pc);
	printf("%04x : %s\n", pc, buffer);
}

static void cmd_register(void)
{
	char* w1 = strtok(NULL, " ");
	char* w2 = strtok(NULL, " ");

	if (w1 && w2)
	{
		Z80_REG_T reg = -1;
		if (strcmp(w1, "sp") == 0)
			reg = SP;
		else if (strcmp(w1, "pc") == 0)
			reg = PC;
		else if (strcmp(w1, "af") == 0)
			reg = AF;
		else if (strcmp(w1, "bc") == 0)
			reg = BC;
		else if (strcmp(w1, "de") == 0)
			reg = DE;
		else if (strcmp(w1, "hl") == 0)
			reg = HL;
		else
		{
			printf("Bad register\n");
			return;
		}

		i8080_write_reg16(reg, strtoul(w2, NULL, 16));
	}

	showregs();
}

static void cmd_break(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint16_t breakpc = strtoul(w1, NULL, 16);
		for (int i=0; i<sizeof(breakpoints)/sizeof(*breakpoints); i++)
		{
			if (breakpoints[i] == 0xffff)
			{
				breakpoints[i] = breakpc;
				return;
			}
		}
		printf("Too many breakpoints\n");
	}
	else
	{
		for (int i=0; i<sizeof(breakpoints)/sizeof(*breakpoints); i++)
		{
			if (breakpoints[i] != 0xffff)
				printf("%04x\n", breakpoints[i]);
		}
	}
}

static void cmd_watch(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint16_t watchaddr = strtoul(w1, NULL, 16);
		for (int i=0; i<sizeof(watchpoints)/sizeof(*watchpoints); i++)
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
		for (int i=0; i<sizeof(watchpoints)/sizeof(*watchpoints); i++)
		{
			struct watchpoint* w = &watchpoints[i];
			if (w->enabled)
				printf("%04x (current value: %02x)\n", w->address, w->value);
		}
	}
}

static void cmd_delete_breakpoint(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint16_t breakpc = strtoul(w1, NULL, 16);
		for (int i=0; i<sizeof(breakpoints)/sizeof(*breakpoints); i++)
		{
			if (breakpoints[i] == breakpc)
			{
				breakpoints[i] = 0xffff;
				return;
			}
		}
		printf("No such breakpoint\n");
	}
}

static void cmd_delete_watchpoint(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
	{
		uint16_t address = strtoul(w1, NULL, 16);
		for (int i=0; i<sizeof(breakpoints)/sizeof(*breakpoints); i++)
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
		uint16_t startaddr = strtoul(w1, NULL, 16);
		uint16_t endaddr = startaddr + strtoul(w2, NULL, 16);
		uint16_t startrounded = startaddr & ~0xf;
		uint16_t endrounded = (endaddr + 0xf) & ~0xf;

		uint16_t p = startrounded;

		while (p < endrounded)
		{
			printf("%04x : ", p);
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

static void cmd_bdos(void)
{
	char* w1 = strtok(NULL, " ");
	if (w1)
		bdosbreak = !!strtoul(w1, NULL, 16);
	else
		printf("break on bdos entry: %s\n", bdosbreak ? "on" : "off");
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
		   "  s               single step\n"
		   "  g               continue\n"
		   "  bdos 0|1        enable break on bdos entry\n"
		   "  trace 0|1       enable tracing\n"
	);
}

static void debug(void)
{
	bool go = false;
	showregs();
	while (!go)
	{
		char cmdline[80];
		printf("debug> ");
		fflush(stdout);
		if (!fgets(cmdline, sizeof(cmdline), stdin))
			exit(0);

		char* token = strtok(cmdline, " \n\r\t");
		if (token != NULL)
		{
			if (strcmp(token, "?") == 0)
				cmd_help();
			else if (strcmp(token, "r") == 0)
				cmd_register();
			else if (strcmp(token, "b") == 0)
				cmd_break();
			else if (strcmp(token, "w") == 0)
				cmd_watch();
			else if (strcmp(token, "db") == 0)
				cmd_delete_breakpoint();
			else if (strcmp(token, "dw") == 0)
				cmd_delete_watchpoint();
			else if (strcmp(token, "m") == 0)
				cmd_memory();
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
			else if (strcmp(token, "bdos") == 0)
				cmd_bdos();
			else if (strcmp(token, "tracing") == 0)
				cmd_tracing();
			else
				printf("Bad command\n");
		}
	}
}

static void sigusr1_cb(int number)
{
	singlestepping = true;
}

void emulator_init(void)
{
	for (int i=0; i<sizeof(breakpoints)/sizeof(*breakpoints); i++)
		breakpoints[i] = 0xffff;

	singlestepping = flag_enter_debugger;

	struct sigaction action = {
		.sa_handler = sigusr1_cb
	};
	sigaction(SIGUSR1, &action, NULL);
}

void emulator_run(void)
{
	for (;;)
	{
		uint16_t pc = i8080_read_reg16(PC);
		if (!singlestepping)
		{
			for (int i=0; i<sizeof(breakpoints)/sizeof(*breakpoints); i++)
				if (pc == breakpoints[i])
					singlestepping = true;
		}
		for (int i=0; i<sizeof(watchpoints)/sizeof(*watchpoints); i++)
		{
			struct watchpoint* w = &watchpoints[i];
			if (w->enabled && (ram[w->address] != w->value))
			{
				printf("\nWatchpoint hit: %04x has changed from %02x to %02x\n",
					w->address, w->value, ram[w->address]);
				w->value = ram[w->address];
				singlestepping = true;
			}
		}

		if (singlestepping)
			debug();
		else if (tracing)
			showregs();

		i8080_exec(1);
	}
}

