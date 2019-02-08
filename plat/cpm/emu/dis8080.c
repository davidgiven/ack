#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "intel_8080_emulator.h"
#include "dis8080.h"

enum
{
    NOTHING,
    CONST8,
    CONST16,
};

struct insn
{
    const char* name;
    int operand;
};

static struct insn insns[0x100] =
{
    /* 00-07 */
    { "nop",            NOTHING },
    { "lxi b, 0x%04x",  CONST16 },
    { "stax b",         NOTHING },
    { "inx b",          NOTHING },
    { "inr b",          NOTHING },
    { "dcr b",          NOTHING },
    { "mvi b, 0x%02x",  CONST8 },
    { "rlc",            NOTHING },

    /* 08-0f */
    { "undef",          NOTHING },
    { "dad b",          NOTHING },
    { "ldax b",         NOTHING },
    { "dcx b",          NOTHING },
    { "inr c",          NOTHING },
    { "dcr c",          NOTHING },
    { "mvi c, 0x%02x",  CONST8 },
    { "rrc",            NOTHING },

    /* 10-17 */
    { "nop",            NOTHING },
    { "lxi d, 0x%04x",  CONST16 },
    { "stax d",         NOTHING },
    { "inx d",          NOTHING },
    { "inr d",          NOTHING },
    { "dcr d",          NOTHING },
    { "mvi d, 0x%02x",  CONST8 },
    { "ral",            NOTHING },

    /* 18-1f */
    { "undef",          NOTHING },
    { "dad d",          NOTHING },
    { "ldax d",         NOTHING },
    { "dcx d",          NOTHING },
    { "inr e",          NOTHING },
    { "dcr e",          NOTHING },
    { "mvi e, 0x%02x",  CONST8 },
    { "rar",            NOTHING },

    /* 20-27 */
    { "nop",            NOTHING },
    { "lxi h, 0x%04x",  CONST16 },
    { "shld 0x%04x",    CONST16 },
    { "inx h",          NOTHING },
    { "inr h",          NOTHING },
    { "dcr h",          NOTHING },
    { "mvi h, 0x%02x",  CONST8 },
    { "daa",            NOTHING },

    /* 28-2f */
    { "undef",          NOTHING },
    { "dad h",          NOTHING },
    { "lhld 0x%04x",    CONST16 },
    { "dcx h",          NOTHING },
    { "inr l",          NOTHING },
    { "dcr l",          NOTHING },
    { "mvi l, 0x%02x",  CONST8 },
    { "cma",            NOTHING },

    /* 30-37 */
    { "nop",            NOTHING },
    { "lxi sp, 0x%04x", CONST16 },
    { "sta 0x%04x",     CONST16 },
    { "inx sp",         NOTHING },
    { "inr m",          NOTHING },
    { "dcr m",          NOTHING },
    { "mvi m, 0x%02x",  CONST8 },
    { "stc",            NOTHING },

    /* 38-3f */
    { "undef",         NOTHING },
    { "dad sp",        NOTHING },
    { "lda 0x%04x",    CONST16 },
    { "dcx sp",        NOTHING },
    { "inr a",         NOTHING },
    { "dcr a",         NOTHING },
    { "mvi a, 0x%02x", CONST8 },
    { "cmc",           NOTHING },

    /* 40-47 */
    { "mov b, b",      NOTHING },
    { "mov b, c",      NOTHING },
    { "mov b, d",      NOTHING },
    { "mov b, e",      NOTHING },
    { "mov b, h",      NOTHING },
    { "mov b, l",      NOTHING },
    { "mov b, m",      NOTHING },
    { "mov b, a",      NOTHING },

    /* 47-4f */
    { "mov c, b",      NOTHING },
    { "mov c, c",      NOTHING },
    { "mov c, d",      NOTHING },
    { "mov c, e",      NOTHING },
    { "mov c, h",      NOTHING },
    { "mov c, l",      NOTHING },
    { "mov c, m",      NOTHING },
    { "mov c, a",      NOTHING },

    /* 50-57 */
    { "mov d, b",      NOTHING },
    { "mov d, c",      NOTHING },
    { "mov d, d",      NOTHING },
    { "mov d, e",      NOTHING },
    { "mov d, h",      NOTHING },
    { "mov d, l",      NOTHING },
    { "mov d, m",      NOTHING },
    { "mov d, a",      NOTHING },

    /* 57-5f */
    { "mov e, b",      NOTHING },
    { "mov e, c",      NOTHING },
    { "mov e, d",      NOTHING },
    { "mov e, e",      NOTHING },
    { "mov e, h",      NOTHING },
    { "mov e, l",      NOTHING },
    { "mov e, m",      NOTHING },
    { "mov e, a",      NOTHING },

    /* 60-67 */
    { "mov h, b",      NOTHING },
    { "mov h, c",      NOTHING },
    { "mov h, d",      NOTHING },
    { "mov h, e",      NOTHING },
    { "mov h, h",      NOTHING },
    { "mov h, l",      NOTHING },
    { "mov h, m",      NOTHING },
    { "mov h, a",      NOTHING },

    /* 67-6f */
    { "mov l, b",      NOTHING },
    { "mov l, c",      NOTHING },
    { "mov l, d",      NOTHING },
    { "mov l, e",      NOTHING },
    { "mov l, h",      NOTHING },
    { "mov l, l",      NOTHING },
    { "mov l, m",      NOTHING },
    { "mov l, a",      NOTHING },

    /* 70-77 */
    { "mov m, b",      NOTHING },
    { "mov m, c",      NOTHING },
    { "mov m, d",      NOTHING },
    { "mov m, e",      NOTHING },
    { "mov m, h",      NOTHING },
    { "mov m, l",      NOTHING },
    { "mov m, m",      NOTHING },
    { "hlt",           NOTHING },

    /* 77-7f */
    { "mov a, b",      NOTHING },
    { "mov a, c",      NOTHING },
    { "mov a, d",      NOTHING },
    { "mov a, e",      NOTHING },
    { "mov a, h",      NOTHING },
    { "mov a, l",      NOTHING },
    { "mov a, m",      NOTHING },
    { "mov a, a",      NOTHING },

    /* 80-87 */
    { "add b",         NOTHING },
    { "add c",         NOTHING },
    { "add d",         NOTHING },
    { "add e",         NOTHING },
    { "add h",         NOTHING },
    { "add l",         NOTHING },
    { "add m",         NOTHING },
    { "add a",         NOTHING },

    /* 88-8f */
    { "adc b",         NOTHING },
    { "adc c",         NOTHING },
    { "adc d",         NOTHING },
    { "adc e",         NOTHING },
    { "adc h",         NOTHING },
    { "adc l",         NOTHING },
    { "adc m",         NOTHING },
    { "adc a",         NOTHING },

    /* 90-97 */
    { "sub b",         NOTHING },
    { "sub c",         NOTHING },
    { "sub d",         NOTHING },
    { "sub e",         NOTHING },
    { "sub h",         NOTHING },
    { "sub l",         NOTHING },
    { "sub m",         NOTHING },
    { "sub a",         NOTHING },

    /* 98-9f */
    { "sbb b",         NOTHING },
    { "sbb c",         NOTHING },
    { "sbb d",         NOTHING },
    { "sbb e",         NOTHING },
    { "sbb h",         NOTHING },
    { "sbb l",         NOTHING },
    { "sbb m",         NOTHING },
    { "sbb a",         NOTHING },

    /* a0-a7 */
    { "ana b",         NOTHING },
    { "ana c",         NOTHING },
    { "ana d",         NOTHING },
    { "ana e",         NOTHING },
    { "ana h",         NOTHING },
    { "ana l",         NOTHING },
    { "ana m",         NOTHING },
    { "ana a",         NOTHING },

    /* a8-af */
    { "xra b",         NOTHING },
    { "xra c",         NOTHING },
    { "xra d",         NOTHING },
    { "xra e",         NOTHING },
    { "xra h",         NOTHING },
    { "xra l",         NOTHING },
    { "xra m",         NOTHING },
    { "xra a",         NOTHING },

    /* b0-b7 */
    { "ora b",         NOTHING },
    { "ora c",         NOTHING },
    { "ora d",         NOTHING },
    { "ora e",         NOTHING },
    { "ora h",         NOTHING },
    { "ora l",         NOTHING },
    { "ora m",         NOTHING },
    { "ora a",         NOTHING },

    /* b8-bf */
    { "cmp b",         NOTHING },
    { "cmp c",         NOTHING },
    { "cmp d",         NOTHING },
    { "cmp e",         NOTHING },
    { "cmp h",         NOTHING },
    { "cmp l",         NOTHING },
    { "cmp m",         NOTHING },
    { "cmp a",         NOTHING },

    /* c0-c7 */
    { "rnz",           NOTHING },
    { "pop b",         NOTHING },
    { "jnz 0x%04x",    CONST16 },
    { "jmp 0x%04x",    CONST16 },
    { "cnz 0x%04x",    CONST16 },
    { "push b",        NOTHING },
    { "adi 0x%02x",    CONST8 },
    { "rst 0",         NOTHING },

    /* c8-cf */
    { "rz",            NOTHING },
    { "ret",           NOTHING },
    { "jz 0x%04x",     CONST16 },
    { "*jmp 0x%04x",   CONST16 },
    { "cz 0x%04x",     CONST16 },
    { "call 0x%04x",   CONST16 },
    { "aci 0x%02x",    CONST8 },
    { "rst 1",         NOTHING },

    /* d0-d7 */
    { "rnc",           NOTHING },
    { "pop d",         NOTHING },
    { "jnc 0x%04x",    CONST16 },
    { "out 0x%02x",    CONST8 },
    { "cnc 0x%04x",    CONST16 },
    { "push d",        NOTHING },
    { "sui 0x%02x",    CONST8 },
    { "rst 2",         NOTHING },

    /* d8-df */
    { "rc",            NOTHING },
    { "*ret",          NOTHING },
    { "jc 0x%04x",     CONST16 },
    { "in 0x%02x",     CONST8 },
    { "cc 0x%04x",     CONST16 },
    { "*call 0x%04x",  CONST16 },
    { "sbi 0x%02x",    CONST8 },
    { "rst 3",         NOTHING },

    /* e0-e7 */
    { "rpo",           NOTHING },
    { "pop h",         NOTHING },
    { "jpo 0x%04x",    CONST16 },
    { "xthl",          NOTHING },
    { "cpo 0x%04x",    CONST16 },
    { "push h",        NOTHING },
    { "ani 0x%02x",    CONST8 },
    { "rst 4",         NOTHING },

    /* e8-ef */
    { "rpe",           NOTHING },
    { "pchl",          NOTHING },
    { "jpe 0x%04x",    CONST16 },
    { "xchg",          NOTHING },
    { "cpe 0x%04x",    CONST16 },
    { "*call 0x%04x",  CONST16 },
    { "xri 0x%02x",    CONST8 },
    { "rst 5",         NOTHING },

    /* f0-f7 */
    { "rp",            NOTHING },
    { "pop psw",       NOTHING },
    { "jp 0x%04x",     CONST16 },
    { "di",            NOTHING },
    { "cp 0x%04x",     CONST16 },
    { "push psw",      NOTHING },
    { "ori 0x%02x",    CONST8 },
    { "rst 6",         NOTHING },

    /* f8-ff */
    { "rm",            NOTHING },
    { "sphl",          NOTHING },
    { "jm 0x%04x",     CONST16 },
    { "ei",            NOTHING },
    { "cm 0x%04x",     CONST16 },
    { "*call 0x%04x",  CONST16 },
    { "cpi 0x%02x",    CONST8 },
    { "rst 7",         NOTHING },
};

uint16_t i8080_disassemble(char* buffer, size_t bufsiz, uint16_t pc)
{
    uint8_t opcode = i8080_read(pc);
	uint8_t p1 = i8080_read(pc+1);
	uint8_t p2 = i8080_read(pc+2);
    struct insn* insn = &insns[opcode];
    uint16_t value = 0;
	const char* left = "";

	snprintf(buffer, bufsiz, "%04x : ", pc);
	pc++;

    switch (insn->operand)
    {
        case NOTHING:
			left = "%02x       : ";
            break;

        case CONST8:
			left = "%02x %02x    : ";
            value = p1;
			pc++;
            break;

        case CONST16:
			left = "%02x %02x %02x : ";
            value = p1 | (p2<<8);
			pc += 2;
            break;
    }

	snprintf(buffer + 7, bufsiz - 7, left, opcode, p1, p2);
    snprintf(buffer + 18, bufsiz - 18, insn->name, value);
    return pc;
}
