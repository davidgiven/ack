#ifndef INTEL_I8080_EMULATOR_H
#define INTEL_I8080_EMULATOR_H

typedef enum
{
	B=0, C, D, E, H, L, M, A, FLAGS,
	AF, BC, DE, HL, SP, PC
}
reg_t;

extern uint8_t i8080_read(uint16_t addr);
extern void i8080_write(uint16_t addr, uint8_t value);
extern uint8_t i8080_inport(uint8_t port);
extern void i8080_outport(uint8_t port, uint8_t value);

extern uint8_t i8080_read_reg8(reg_t reg);
extern void i8080_write_reg8(reg_t reg, uint8_t value);

extern uint16_t i8080_read_reg16(reg_t reg);
extern void i8080_write_reg16(reg_t reg, uint16_t value);

extern int i8080_exec(int cycles);

#endif

