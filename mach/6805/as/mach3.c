/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID3 "$Id$"

/*
 * Motorola 6805 keywords
 */
/*
 * The X register
 */
0,	X,		0,		"x",
/*
 * Bit test and branch
 */
0,	BBRANCH,	0x00,		"brset",
0,	BBRANCH,	0x01,		"brclr",
/*
 * Bit manipulation
 */
0,	BIT,		0x10,		"bset",
0,	BIT,		0x11,		"bclr",
/*
 * Branches
 */
0,	BRANCH,		0x20,		"bra",
0,	BRANCH,		0x21,		"brn",
0,	BRANCH,		0x22,		"bhi",
0,	BRANCH,		0x23,		"bls",
0,	BRANCH,		0x24,		"bcc",
0,	BRANCH,		0x25,		"bcs",
0,	BRANCH,		0x26,		"bne",
0,	BRANCH,		0x27,		"beq",
0,	BRANCH,		0x28,		"bhcc",
0,	BRANCH,		0x29,		"bhcs",
0,	BRANCH,		0x2a,		"bpl",
0,	BRANCH,		0x2b,		"bmi",
0,	BRANCH,		0x2c,		"bmc",
0,	BRANCH,		0x2d,		"bms",
0,	BRANCH,		0x2e,		"bil",
0,	BRANCH,		0x2f,		"bih",
/*
 * Read modify write on anything but registers
 */
0,	RMR,		0x30,		"neg",
0,	RMR,		0x33,		"com",
0,	RMR,		0x34,		"lsr",
0,	RMR,		0x36,		"ror",
0,	RMR,		0x36,		"asr",
0,	RMR,		0x38,		"lsl",
0,	RMR,		0x39,		"rol",
0,	RMR,		0x3a,		"dec",
0,	RMR,		0x3c,		"inc",
0,	RMR,		0x3d,		"tst",
0,	RMR,		0x3f,		"clr",
/*
 * Implied stuff
 */
0,	NOARG,		0x80,		"rti",
0,	NOARG,		0x81,		"rts",
0,	NOARG,		0x83,		"swi",
0,	NOARG,		0x97,		"tax",
0,	NOARG,		0x98,		"clc",
0,	NOARG,		0x99,		"sec",
0,	NOARG,		0x9a,		"cli",
0,	NOARG,		0x9b,		"sei",
0,	NOARG,		0x9c,		"rsp",
0,	NOARG,		0x9d,		"nop",
0,	NOARG,		0x9f,		"txa",
/*
 * Register memory.
 * Warning. Some imediate opcodes excluded in parser actions.
 */
0,	RM,		0xa0,		"sub",
0,	RM,		0xa1,		"cmp",
0,	RM,		0xa2,		"sbc",
0,	RM,		0xa3,		"cpx",
0,	RM,		0xa4,		"and",
0,	RM,		0xa5,		"bit",
0,	RM,		0xa6,		"lda",
0,	RM,		0xa7,		"sta",
0,	RM,		0xa8,		"eor",
0,	RM,		0xa9,		"adc",
0,	RM,		0xaa,		"ora",
0,	RM,		0xab,		"add",
0,	RM,		0xac,		"jmp",
0,	BRANCH,		0xad,		"bsr",
0,	RM,		0xad,		"jsr",
0,	RM,		0xae,		"ldx",
0,	RM,		0xaf,		"stx",
/*
 * Branch synonyms
 */
0,	BRANCH,		0x24,		"bhs",	/* bcc */
0,	BRANCH,		0x25,		"blo",	/* bcs */
/*
 * Brain damaged concatenated opcodes for RMR on registers
 */
0,	NOARG,		0x40,		"nega",
0,	NOARG,		0x43,		"coma",
0,	NOARG,		0x44,		"lsra",
0,	NOARG,		0x46,		"rora",
0,	NOARG,		0x47,		"asra",
0,	NOARG,		0x48,		"lsla",
0,	NOARG,		0x49,		"rola",
0,	NOARG,		0x4a,		"deca",
0,	NOARG,		0x4c,		"inca",
0,	NOARG,		0x4d,		"tsta",
0,	NOARG,		0x4f,		"clra",
0,	NOARG,		0x50,		"negx",
0,	NOARG,		0x53,		"comx",
0,	NOARG,		0x54,		"lsrx",
0,	NOARG,		0x56,		"rorx",
0,	NOARG,		0x57,		"asrx",
0,	NOARG,		0x58,		"lslx",
0,	NOARG,		0x59,		"rolx",
0,	NOARG,		0x5a,		"decx",
0,	NOARG,		0x5c,		"incx",
0,	NOARG,		0x5d,		"tstx",
0,	NOARG,		0x5f,		"clrx",
/*
 * CMOS support
 */
0,	CMOS,		0,		".cmos",
0,	CMOS,		0x8e,		"stop",
0,	CMOS,		0x8f,		"wait",
