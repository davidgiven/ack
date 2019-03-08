/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID3 "$Id$"

/*
 * Specials
 */

0,  KILL,       0,      "kill",

/*
 * Intel 8080 keywords
 */
0,	REG,		B,		"b",
0,	REG,		C,		"c",
0,	REG,		D,		"d",
0,	REG,		E,		"e",
0,	REG,		H,		"h",
0,	REG,		L,		"l",
0,	REG,		M,		"m",
0,	REG,		A,		"a",
0,	REG,		SP,		"sp",
0,	REG,		PSW,		"psw",
0,	D16OP,		0315,		"call",
0,	D16OP,		0334,		"cc",
0,	D16OP,		0324,		"cnc",
0,	D16OP,		0314,		"cz",
0,	D16OP,		0304,		"cnz",
0,	D16OP,		0364,		"cp",
0,	D16OP,		0374,		"cm",
0,	D16OP,		0354,		"cpe",
0,	D16OP,		0344,		"cpo",
0,	NOOPOP,		0311,		"ret",
0,	NOOPOP,		0330,		"rc",
0,	NOOPOP,		0320,		"rnc",
0,	NOOPOP,		0310,		"rz",
0,	NOOPOP,		0300,		"rnz",
0,	NOOPOP,		0360,		"rp",
0,	NOOPOP,		0370,		"rm",
0,	NOOPOP,		0350,		"rpe",
0,	NOOPOP,		0340,		"rpo",
0,	RST,		0307,		"rst",
0,	D8OP,		0333,		"in",
0,	D8OP,		0323,		"out",
0,	LXI,		0001,		"lxi",
0,	R16OP,		0305,		"push",
0,	R16OP,		0301,		"pop",
0,	D16OP,		0062,		"sta",
0,	D16OP,		0072,		"lda",
0,	NOOPOP,		0353,		"xchg",
0,	NOOPOP,		0343,		"xthl",
0,	NOOPOP,		0371,		"sphl",
0,	NOOPOP,		0351,		"pchl",
0,	R16OP,		0011,		"dad",
0,	STLDAX,		0002,		"stax",
0,	STLDAX,		0012,		"ldax",
0,	R16OP,		0003,		"inx",
0,	MOV,		0100,		"mov",
0,	NOOPOP,		0166,		"hlt",
0,	MVI,		0006,		"mvi",
0,	DSTOP,		0004,		"inr",
0,	DSTOP,		0005,		"dcr",
0,	SRCOP,		0200,		"add",
0,	SRCOP,		0210,		"adc",
0,	SRCOP,		0220,		"sub",
0,	SRCOP,		0230,		"sbb",
0,	SRCOP,		0240,		"ana",
0,	SRCOP,		0250,		"xra",
0,	SRCOP,		0260,		"ora",
0,	SRCOP,		0270,		"cmp",
0,	D8OP,		0306,		"adi",
0,	D8OP,		0316,		"aci",
0,	D8OP,		0326,		"sui",
0,	D8OP,		0336,		"sbi",
0,	D8OP,		0346,		"ani",
0,	D8OP,		0356,		"xri",
0,	D8OP,		0366,		"ori",
0,	D8OP,		0376,		"cpi",
0,	NOOPOP,		0007,		"rlc",
0,	NOOPOP,		0017,		"rrc",
0,	NOOPOP,		0027,		"ral",
0,	NOOPOP,		0037,		"rar",
0,	D16OP,		0303,		"jmp",
0,	D16OP,		0332,		"jc",
0,	D16OP,		0322,		"jnc",
0,	D16OP,		0312,		"jz",
0,	D16OP,		0302,		"jnz",
0,	D16OP,		0362,		"jp",
0,	D16OP,		0372,		"jm",
0,	D16OP,		0352,		"jpe",
0,	D16OP,		0342,		"jpo",
0,	R16OP,		0013,		"dcx",
0,	NOOPOP,		0057,		"cma",
0,	NOOPOP,		0067,		"stc",
0,	NOOPOP,		0077,		"cmc",
0,	NOOPOP,		0047,		"daa",
0,	D16OP,		0042,		"shld",
0,	D16OP,		0052,		"lhld",
0,	NOOPOP,		0373,		"ei",
0,	NOOPOP,		0363,		"di",
0,	NOOPOP,		0000,		"nop",
0,	NOOPOP,		0040,		"rim",		/* 8085 */
0,	NOOPOP,		0060,		"sim",		/* 8085 */
/*
 *	The Intel 8085 has a set of instructions for high level language
 *	support that were not originally documented.
 *
 *	V = signed overflow (carry out ^ carry in)
 *	K = signed compare (v ^ sign)
 *
 *	K is mostly useful for oveflowing INX and DEX to avoid the usual
 *	check via A. RSTV is useful for runtimes where signed overflow is a
 *	fault.
 */
0,	NOOPOP,		0010,		"dsub",		/* 8085 undoc HL = HL - BC */
0,	NOOPOP,		0020,		"arhl",		/* 8085 undoc HL arith right shift */
0,	NOOPOP,		0030,		"rld",		/* 8085 undoc rotate DE left thru carry */
                                                        /* in effect ADC DE,DE */
0,	D8OP,		0050,		"ldhi",		/* DE = HL + imm8 */
0,	D8OP,		0070,		"ldsi",		/* DE = SP + imm8 */
0,	NOOPOP,		0313,		"rstv",		/* RST8 if V set */
0,	NOOPOP,		0331,		"shlx",		/* (DE) = HL */
0,	NOOPOP,		0355,		"lhlx",		/* HL = (DE) */
0,	D16OP,		0335,		"jnk",		/* Jump K clear */
0,	D16OP,		0375,		"jk",		/* Jump K set */
