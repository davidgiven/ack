/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID3 "$Id$"

/*
** Zilog z8000 keywords
*/
0,		R8,		8,		"RL0",
0,		R8,		0,		"RH0",
0,		R8,		9,		"RL1",
0,		R8,		1,		"RH1",
0,		R8,		10,		"RL2",
0,		R8,		2,		"RH2",
0,		R8,		11,		"RL3",
0,		R8,		3,		"RH3",
0,		R8,		12,		"RL4",
0,		R8,		4,		"RH4",
0,		R8,		13,		"RL5",
0,		R8,		5,		"RH5",
0,		R8,		14,		"RL6",
0,		R8,		6,		"RH6",
0,		R8,		15,		"RL7",
0,		R8,		7,		"RH7",
/* Special format for some byte-registers.  Not really available on
** the z8000 but designed to ease writing a z8000-backend-table.
** LR[0..7] are equivalent with RL[0..7].
*/
0,		R8,		8,		"LR0",
0,		R8,		9,		"LR1",
0,		R8,		10,		"LR2",
0,		R8,		11,		"LR3",
0,		R8,		12,		"LR4",
0,		R8,		13,		"LR5",
0,		R8,		14,		"LR6",
0,		R8,		15,		"LR7",
0,		R16,		0,		"R0",
0,		R16,		1,		"R1",
0,		R16,		2,		"R2",
0,		R16,		3,		"R3",
0,		R16,		4,		"R4",
0,		R16,		5,		"R5",
0,		R16,		6,		"R6",
0,		R16,		7,		"R7",
0,		R16,		8,		"R8",
0,		R16,		9,		"R9",
0,		R16,		10,		"R10",
0,		R16,		11,		"R11",
0,		R16,		12,		"R12",
0,		R16,		13,		"R13",
0,		R16,		14,		"R14",
0,		R16,		15,		"R15",
0,		R32,		0,		"RR0",
0,		R32,		2,		"RR2",
0,		R32,		4,		"RR4",
0,		R32,		6,		"RR6",
0,		R32,		8,		"RR8",
0,		R32,		10,		"RR10",
0,		R32,		12,		"RR12",
0,		R32,		14,		"RR14",
0,		R64,		0,		"RQ0",
0,		R64,		4,		"RQ4",
0,		R64,		8,		"RQ8",
0,		R64,		12,		"RQ12",
0,		CC,		14,		"NZ",
0,		CC,		15,		"NC",
0,		CC,		13,		"PL",
0,		CC,		5,		"MI",
0,		CC,		14,		"NE",
0,		CC,		6,		"EQ",
0,		CC,		4,		"OV",
0,		CC,		12,		"NOV",
0,		CC,		4,		"PE",
0,		CC,		12,		"PO",
0,		CC,		9,		"GE",
0,		CC,		1,		"LT",
0,		CC,		10,		"GT",
0,		CC,		2,		"LE",
0,		CC,		15,		"UGE",
0,		CC,		7,		"ULT",
0,		CC,		11,		"UGT",
0,		CC,		3,		"ULE",
0,		FLAG,		0x80,		"C",
0,		FLAG,		0x40,		"Z",
0,		FLAG,		0x20,		"S",
0,		FLAG,		0x10,		"P",
0,		FLAG,		0x10,		"V",
0,		INTCB,		2,		"VI",
0,		INTCB,		1,		"NVI",
0,		CTLRFLAGS,	1,		"FLAGS",
0,		CTLR,		2,		"FCW",
0,		CTLR,		3,		"REFRESH",
0,		CTLR,		4,		"PSAPSEG",
0,		CTLR,		5,		"PSAPOFF",
0,		CTLR,		6,		"NSPSEG",
0,		CTLR,		7,		"NSPOFF",
0,		CTLR,		5,		"PSAP",
0,		CTLR,		7,		"NSP",

	/* TYPE_11a23	*/
0,		F1_1F2_3,	0x1F00,		"call",
0,		F1_1F2_3,	0x3900,		"ldps",
	/* TYPE_11b23	*/
0,		F1_1F2_3,	0x0D08,		"clr",
0,		F1_1F2_3,	0x0C08,		"clrb",
0,		F1_1F2_3,	0x0D00,		"com",
0,		F1_1F2_3,	0x0C00,		"comb",
0,		F1_1F2_3,	0x0D02,		"neg",
0,		F1_1F2_3,	0x0C02,		"negb",
0,		F1_1F2_3,	0x0D04,		"test",
0,		F1_1F2_3,	0x0C04,		"testb",
0,		F1_1F2_3,	0x1C08,		"testl",
0,		F1_1F2_3,	0x0D06,		"tset",
0,		F1_1F2_3,	0x0C06,		"tsetb",

0,		F1_1a,		0xB000,		"dab",
0,		F1_1a,		0xB10A,		"exts",
0,		F1_1a,		0xB100,		"extsb",
0,		F1_1a,		0xB107,		"extsl",

0,		F1_1b,		0xB300,		"rl",
0,		F1_1b,		0xB200,		"rlb",
0,		F1_1b,		0xB308,		"rlc",
0,		F1_1b,		0xB208,		"rlcb",
0,		F1_1b,		0xB304,		"rr",
0,		F1_1b,		0xB204,		"rrb",
0,		F1_1b,		0xB30C,		"rrc",
0,		F1_1b,		0xB20C,		"rrcb",

	/* TYPE_12	*/
0,		F1_2,		0x2B00,		"dec",
0,		F1_2,		0x2A00,		"decb",
0,		F1_2,		0x2900,		"inc",
0,		F1_2,		0x2800,		"incb",
0,		LDK,		0xBD00,		"ldk",
	/* TYPE_1263	*/
0,		F1_2F6_3,	0x2700,		"bit",
0,		F1_2F6_3,	0x2600,		"bitb",
0,		F1_2F6_3,	0x2300,		"res",
0,		F1_2F6_3,	0x2200,		"resb",
0,		F1_2F6_3,	0x2500,		"set",
0,		F1_2F6_3,	0x2400,		"setb",

	/* TYPE_jp	*/
0,		JP,		0x1E00,		"jp",

0,		TCC,		0xAF00,		"tcc",
0,		TCC,		0xAE00,		"tccb",

	/* TYPE_21a	*/
0,		F2_1,		0x2D00,		"ex",
0,		F2_1,		0x2C00,		"exb",
	/* TYPE_21b	*/
0,		F2_1,		0x3500,		"adc",
0,		F2_1,		0x3400,		"adcb",
0,		F2_1,		0x3E00,		"rldb",
0,		F2_1,		0x3C00,		"rrdb",
0,		F2_1,		0x3700,		"sbc",
0,		F2_1,		0x3600,		"sbcb",
	/* TYPE_2151.
	**      Depending on their operands the cp-instructions might
	** have an opcode of 0x201 more then listed below.    This is
	** added at the appropriate place.
	** 	The difference in opcode between byte-,word- and long-
	** instructions of the F2_1F5_1 group is as follows:
	**   If bit 8 is on it is a word instruction;  If it is not a
	** word instruction and bit 12 is on it is a long instruction,
	** else it is a byte instruction.    This information is used
	** when one of the operands is of type IM.
	*/
0,		F2_1F5_1,	0x0100,		"add",
0,		F2_1F5_1,	0x0000,		"addb",
0,		F2_1F5_1,	0x1600,		"addl",
0,		F2_1F5_1,	0x0700,		"and",
0,		F2_1F5_1,	0x0600,		"andb",
0,		F2_1F5_1,	0x1B00,		"div",
0,		F2_1F5_1,	0x1A00,		"divl",
0,		F2_1F5_1,	0x1900,		"mult",
0,		F2_1F5_1,	0x1800,		"multl",
0,		F2_1F5_1,	0x0500,		"or",
0,		F2_1F5_1,	0x0400,		"orb",
0,		F2_1F5_1,	0x0300,		"sub",
0,		F2_1F5_1,	0x0200,		"subb",
0,		F2_1F5_1,	0x1200,		"subl",
0,		F2_1F5_1,	0x0900,		"xor",
0,		F2_1F5_1,	0x0800,		"xorb",
0,		F2_1F5_1,	0x0B00,		"cp",
0,		F2_1F5_1,	0x0A00,		"cpb",
0,		F2_1F5_1,	0x1000,		"cpl",

0,		LDA,		0,		"lda",
	/* TYPE_pop	*/
0,		POP,		0x1700,		"pop",
0,		POP,		0x1500,		"popl",
	/* TYPE_push	*/
0,		PUSH,		0x1300,		"push",
0,		PUSH,		0x1100,		"pushl",

	/* TYPE_ld	*/
0,		LD,		0x0100,		"ld",
0,		LD,		0,		"ldb",
0,		LDL,		0,		"ldl",

0,		DJNZ,		0xF080,		"djnz",
0,		DJNZ,		0xF000,		"dbjnz",
0,		JR,		0xE000,		"jr",
0,		CALR,		0xD000,		"calr",

	/*     Depending on their operands the LDR-instructions might
	** have an opcode of 0x200 more then listed below.    This is
	** or-ed in at the appropriate place.
	*/
0,		LDR,		0x3100,		"ldr",
0,		LDR,		0x3000,		"ldrb",
0,		LDR,		0x3500,		"ldrl",

0,		LDAR,		0x3400,		"ldar",

0,		F5_1L,		0xB309,		"sla",
0,		F5_1L,		0xB209,		"slab",
0,		F5_1L,		0xB30D,		"slal",
0,		F5_1L,		0xB301,		"sll",
0,		F5_1L,		0xB201,		"sllb",
0,		F5_1L,		0xB305,		"slll",
0,		F5_1R,		0xB309,		"sra",
0,		F5_1R,		0xB209,		"srab",
0,		F5_1R,		0xB30D,		"sral",
0,		F5_1R,		0xB301,		"srl",
0,		F5_1R,		0xB201,		"srlb",
0,		F5_1R,		0xB305,		"srll",

	/*    Depending on its operands the LDM-instruction might have
	** an opcode of 8 more then listed below.  This is added at the
	** appropriate place.
	** TYPE_ldm
	*/
0,		LDM,		0x1C01,		"ldm",

	/*   For the F6.4 instructions below the yylval-column contains
	** the opcode for the instruction. However the third hexa-digit
	** should be 0;   But this is the opcode which must be put into
	** the second word of the instruction!
	*/
0,		F6_4,		0x3B88,		"ind",
0,		F6_4,		0x3A88,		"indb",
0,		F6_4,		0x3B08,		"indr",
0,		F6_4,		0x3A08,		"indrb",
0,		F6_4,		0x3B80,		"ini",
0,		F6_4,		0x3A80,		"inib",
0,		F6_4,		0x3B00,		"inir",
0,		F6_4,		0x3A00,		"inirb",
0,		F6_4,		0xBB89,		"ldd",
0,		F6_4,		0xBA89,		"lddb",
0,		F6_4,		0xBB09,		"lddr",
0,		F6_4,		0xBA09,		"lddrb",
0,		F6_4,		0xBB81,		"ldi",
0,		F6_4,		0xBA81,		"ldib",
0,		F6_4,		0xBB01,		"ldir",
0,		F6_4,		0xBA01,		"ldirb",
0,		F6_4,		0x3B0A,		"otdr",
0,		F6_4,		0x3A0A,		"otdrb",
0,		F6_4,		0x3B02,		"otir",
0,		F6_4,		0x3A02,		"otirb",
0,		F6_4,		0x3B8A,		"outd",
0,		F6_4,		0x3A8A,		"outdb",
0,		F6_4,		0x3B82,		"outi",
0,		F6_4,		0x3A82,		"outib",
0,		F6_4,		0x3B89,		"sind",
0,		F6_4,		0x3A89,		"sindb",
0,		F6_4,		0x3B09,		"sindr",
0,		F6_4,		0x3A09,		"sindrb",
0,		F6_4,		0x3B81,		"sini",
0,		F6_4,		0x3A81,		"sinib",
0,		F6_4,		0x3B01,		"sinir",
0,		F6_4,		0x3A01,		"sinirb",
0,		F6_4,		0x3B0B,		"sotdr",
0,		F6_4,		0x3A0B,		"sotdrb",
0,		F6_4,		0x3B03,		"sotir",
0,		F6_4,		0x3A03,		"sotirb",
0,		F6_4,		0x3B8B,		"soutd",
0,		F6_4,		0x3A8B,		"soutdb",
0,		F6_4,		0x3B83,		"souti",
0,		F6_4,		0x3A83,		"soutib",
0,		F6_4,		0xB808,		"trdb",
0,		F6_4,		0xB80C,		"trdrb",
0,		F6_4,		0xB800,		"trib",
0,		F6_4,		0xB804,		"trirb",
0,		F6_4,		0xB80A,		"trtdb",
0,		F6_4,		0xB8EE,		"trtdrb",
0,		F6_4,		0xB802,		"trtib",
0,		F6_4,		0xB8E6,		"trtirb",

	/*   From the F6.5 instructions below the last eight ('string'-
	** instructions) want an 'ir' as operand; The others want a 'r'.
	** In the opcode for the string-instructions bit 1 is on, which
	** indicates the difference.
	*/
0,		F6_5,		0xBB08,		"cpd",
0,		F6_5,		0xBA08,		"cpdb",
0,		F6_5,		0xBB0C,		"cpdr",
0,		F6_5,		0xBA0C,		"cpdrb",
0,		F6_5,		0xBB00,		"cpi",
0,		F6_5,		0xBA00,		"cpib",
0,		F6_5,		0xBB04,		"cpir",
0,		F6_5,		0xBA04,		"cpirb",
0,		F6_5,		0xBB0A,		"cpsd",
0,		F6_5,		0xBA0A,		"cpsdb",
0,		F6_5,		0xBB0E,		"cpsdr",
0,		F6_5,		0xBA0E,		"cpsdrb",
0,		F6_5,		0xBB02,		"cpsi",
0,		F6_5,		0xBA02,		"cpsib",
0,		F6_5,		0xBB06,		"cpsir",
0,		F6_5,		0xBA06,		"cpsirb",

0,		F6_6,		0xB30B,		"sda",
0,		F6_6,		0xB20B,		"sdab",
0,		F6_6,		0xB30F,		"sdal",
0,		F6_6,		0xB303,		"sdl",
0,		F6_6,		0xB203,		"sdlb",
0,		F6_6,		0xB307,		"sdll",

	/* The instructions in\b and out\b have two different opcodes
	** depending on their operands (...).   Therefore the opcodes
	** below are not complete.    The rest is or-ed in at the ap-
	** propriate place!
	**	rest	|  r and da	r and ir
	**	---------------------------------
	**	in\b	|   0xA04	0xC00
	**	out\b	|   0xA06	OxE00
	** Furthermore the 'special'-instructions don't allow an 'ir'
	** as operand.   In their opcode bit 0 is on, which indicates
	** the difference with the other instructions of this group.
	*/
0,		IN,		0x3100,		"in",
0,		IN,		0x3000,		"inb",
0,		IN,		0x3B05,		"sin",
0,		IN,		0x3A05,		"sinb",
0,		OUT,		0x3100,		"out",
0,		OUT,		0x3000,		"outb",
0,		OUT,		0x3B07,		"sout",
0,		OUT,		0x3A07,		"soutb",

	/*   Depending on their operands the LDCTL-instructions might
	** have an opcode of 8 more then listed below.  This is or-ed
	** in at the appropriate place.
	*/
0,		LDCTL,		0x7D00,		"ldctl",
0,		LDCTLB,		0x8C00,		"ldctlb",
0,		MREQ,		0x7B0D,		"mreq",

0,		F9_1,		0x8D05,		"comflg",
0,		F9_1,		0x8D03,		"resflg",
0,		F9_1,		0x8D01,		"setflg",

0,		F9_2,		0x7C00,		"di",
0,		F9_2,		0x7C04,		"ei",

0,		F9_3,		0x7A00,		"halt",
0,		F9_3,		0x7B00,		"iret",
0,		F9_3,		0x7B0A,		"mbit",
0,		F9_3,		0x7B09,		"mres",
0,		F9_3,		0x7B08,		"mset",
0,		F9_3,		0x8D07,		"nop",

	/* Rest of the opcode-0x200 is or-ed in at the appropriate place
	*/
0,		RET,		0x9E00,		"ret",
0,		SC,		0x7F00,		"sc",
