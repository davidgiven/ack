#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define RAMSIZE (1*1024*1024)

extern uint8_t ram[RAMSIZE];

extern void fatal(const char* message, ...);

extern uint8_t mem_read_cpu(uint32_t address, bool code);
extern void mem_write_cpu(uint32_t address, uint8_t value);

extern uint8_t port_read_byte(uint32_t address);
extern void port_write_byte(uint32_t address, uint8_t value);

#endif

