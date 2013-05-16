/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "binary.h"

/* Integer registers */

0,     GPR,        0,          "r0",
0,     GPR,        1,          "r1",
0,     GPR,        2,          "r2",
0,     GPR,        3,          "r3",
0,     GPR,        4,          "r4",
0,     GPR,        5,          "r5",

0,     GPR,        6,          "r6",
0,     GPR,        6,          "fp",
0,     GPR,        7,          "r7",
0,     GPR,        8,          "r8",
0,     GPR,        9,          "r9",
0,     GPR,        10,         "r10",
0,     GPR,        11,         "r11",
0,     GPR,        12,         "r12",
0,     GPR,        13,         "r13",
0,     GPR,        14,         "r14",
0,     GPR,        15,         "r15",
0,     GPR,        16,         "r16",
0,     GPR,        17,         "r17",
0,     GPR,        18,         "r18",
0,     GPR,        19,         "r19",
0,     GPR,        20,         "r20",
0,     GPR,        21,         "r21",
0,     GPR,        22,         "r22",
0,     GPR,        23,         "r23",
0,     GPR,        24,         "r24",

0,     GPR,        25,         "r25",
0,     GPR,        25,         "sp",
0,     GPR,        26,         "r26",
0,     GPR,        26,         "lr",
0,     GPR,        27,         "r27",
0,     GPR,        28,         "r28",
0,     GPR,        29,         "r29",
0,     GPR,        30,         "r30",
0,     GPR,        30,         "sr",
0,     GPR,        31,         "r31",
0,     GPR,        31,         "pc",

/* Special instructions */

0,     OP,                    B16(00000000,00000001),                  "nop",
0,     OP,                    B16(00000000,00001010),                  "rti",

0,     OP_ONEREG,             B16(00000000,01000000),                  "b",
0,     OP_ONEREG,             B16(00000000,01100000),                  "bl",
0,     OP_ONELREG,            B16(00000000,10000000),                  "tbb",
0,     OP_ONELREG,            B16(00000000,10100000),                  "tbs",

0,     OP_ALU,                B8(00000000),                            "mov",
0,     OP_ALU,                B8(00000001),                            "cmn",
0,     OP_ALU,                B8(00000010),                            "add",
0,     OP_ALU,                B8(00000011),                            "bic",
0,     OP_ALU,                B8(00000100),                            "mul",
0,     OP_ALU,                B8(00000101),                            "eor",
0,     OP_ALU,                B8(00000110),                            "sub",
0,     OP_ALU,                B8(00000111),                            "and",
0,     OP_ALU,                B8(00001000),                            "mvn",
0,     OP_ALU,                B8(00001001),                            "ror",
0,     OP_ALU,                B8(00001010),                            "cmp",
0,     OP_ALU,                B8(00001011),                            "rsb",
0,     OP_ALU,                B8(00001100),                            "btst",
0,     OP_ALU,                B8(00001101),                            "or",
0,     OP_ALU,                B8(00001110),                            "extu",
0,     OP_ALU,                B8(00001111),                            "max",
0,     OP_ALU,                B8(00010000),                            "bset",
0,     OP_ALU,                B8(00010001),                            "min",
0,     OP_ALU,                B8(00010010),                            "bclr",
0,     OP_ALU,                B8(00010011),                            "adds2",
0,     OP_ALU,                B8(00010100),                            "bchg",
0,     OP_ALU,                B8(00010101),                            "adds4",
0,     OP_ALU,                B8(00010110),                            "adds8",
0,     OP_ALU,                B8(00010111),                            "adds16",
0,     OP_ALU,                B8(00011000),                            "exts",
0,     OP_ALU,                B8(00011001),                            "neg",
0,     OP_ALU,                B8(00011010),                            "lsr",
0,     OP_ALU,                B8(00011011),                            "clz",
0,     OP_ALU,                B8(00011100),                            "lsl",
0,     OP_ALU,                B8(00011101),                            "brev",
0,     OP_ALU,                B8(00011110),                            "asr",
0,     OP_ALU,                B8(00011111),                            "abs",


