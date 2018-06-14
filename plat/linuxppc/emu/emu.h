#ifndef EMU_H
#define EMU_H

extern void fatal(char* fmt, ...);

typedef struct
{
	uint32_t gpr[32];
	uint64_t fpr[32];
	uint32_t cr;
	uint32_t ctr;
	uint32_t lr;
	uint32_t xer;
	uint32_t fpscr;
	uint32_t cia;
	uint32_t nia;
}
cpu_t;

extern cpu_t cpu;

extern uint32_t read_byte(uint32_t address);
extern uint32_t read_word(uint32_t address);
extern uint32_t read_long(uint32_t address);
extern void write_byte(uint32_t address, uint32_t value);
extern void write_word(uint32_t address, uint32_t value);
extern void write_long(uint32_t address, uint32_t value);

extern void system_call(uint8_t trapno);

#endif

