/*
 * Motorola 68020 keywords
 */

0,	SIZE,		SIZE_B,		".b",
0,	SIZE,		SIZE_W,		".w",
0,	SIZE,		SIZE_L,		".l",

0,	DREG,		00,		"d0",
0,	DREG,		01,		"d1",
0,	DREG,		02,		"d2",
0,	DREG,		03,		"d3",
0,	DREG,		04,		"d4",
0,	DREG,		05,		"d5",
0,	DREG,		06,		"d6",
0,	DREG,		07,		"d7",

0,	AREG,		00,		"a0",
0,	AREG,		01,		"a1",
0,	AREG,		02,		"a2",
0,	AREG,		03,		"a3",
0,	AREG,		04,		"a4",
0,	AREG,		05,		"a5",
0,	AREG,		06,		"a6",
0,	AREG,		07,		"a7",
0,	AREG,		07,		"sp",

0,	PC,		0,		"pc",
0,	ZPC,		0,		"zpc",

0,	CREG,		04001,		"vbr",
0,	CREG,		0,		"sfc",
0,	CREG,		00001,		"dfc",
0,	CREG,		00002,		"cacr",
0,	CREG,		04002,		"caar",
0,	CREG,		04003,		"msp",
0,	CREG,		04004,		"isp",

0,	SPEC,		075,		"usp",
0,	SPEC,		076,		"ccr",
0,	SPEC,		077,		"sr",

0,	ABCD,		0140400,	"abcd",
0,	ABCD,		0100400,	"sbcd",

0,	ADDX,		0150400,	"addx",
0,	ADDX,		0110400,	"subx",

0,	ADD,		0153300,	"add",
0,	ADD,		0112700,	"sub",

0,	AND,		0141000,	"and",
0,	AND,		0135000,	"eor",
0,	AND,		0100000,	"or",

0,	BITOP,		0000,		"btst",
0,	BITOP,		0100,		"bchg",
0,	BITOP,		0200,		"bclr",
0,	BITOP,		0300,		"bset",

0,	BITFIELD,	0164300,	"bftst",
0,	BITFIELD,	0165300,	"bfchg",
0,	BITFIELD,	0166300,	"bfclr",
0,	BITFIELD,	0167300,	"bfset",
0,	BF_TO_D,	0164700,	"bfextu",
0,	BF_TO_D,	0164700,	"bfexts",
0,	BF_TO_D,	0164700,	"bfffo",
0,	BFINS,		0167700,	"bfins",

0,	SHIFT,		0160340,	"asr",
0,	SHIFT,		0160740,	"asl",
0,	SHIFT,		0161350,	"lsr",
0,	SHIFT,		0161750,	"lsl",
0,	SHIFT,		0162360,	"roxr",
0,	SHIFT,		0162760,	"roxl",
0,	SHIFT,		0163370,	"ror",
0,	SHIFT,		0163770,	"rol",

0,	SZ_EA,		041000|DTA|ALT,	"clr",
0,	SZ_EA,		042000|DTA|ALT,	"neg",
0,	SZ_EA,		040000|DTA|ALT,	"negx",
0,	SZ_EA,		043000|DTA|ALT,	"not",
0,	SZ_EA,		045000|DTA,	"tst",

0,	OP_EA,		044000|DTA|ALT,	"nbcd",
0,	OP_EA,		045300|DTA|ALT,	"tas",
0,	OP_EA,		047200|CTR,	"jsr",
0,	OP_EA,		047300|CTR,	"jmp",
0,	OP_EA,		044100|CTR,	"pea",

0,	OP_EA,		050300,		"st",
0,	OP_EA,		050700,		"sf",
0,	OP_EA,		051300,		"shi",
0,	OP_EA,		051700,		"sls",
0,	OP_EA,		052300,		"scc",
0,	OP_EA,		052700,		"scs",
0,	OP_EA,		053300,		"sne",
0,	OP_EA,		053700,		"seq",
0,	OP_EA,		054300,		"svc",
0,	OP_EA,		054700,		"svs",
0,	OP_EA,		055300,		"spl",
0,	OP_EA,		055700,		"smi",
0,	OP_EA,		056300,		"sge",
0,	OP_EA,		056700,		"slt",
0,	OP_EA,		057300,		"sgt",
0,	OP_EA,		057700,		"sle",

0,	OP_NOOP,	047160,		"reset",
0,	OP_NOOP,	047161,		"nop",
0,	OP_NOOP,	047163,		"rte",
0,	OP_NOOP,	047165,		"rts",
0,	OP_NOOP,	047166,		"trapv",
0,	OP_NOOP,	047167,		"rtr",
0,	OP_NOOP,	045374,		"illegal",

0,	PACK,		0100500,	"pack",
0,	PACK,		0100600,	"unpk",

0,	DIVMUL,		0100,		"divu",
0,	DIVMUL,		0101,		"divs",
0,	DIVMUL,		0000,		"mulu",
0,	DIVMUL,		0001,		"muls",

0,	DIVL,		046100,		"divul",
0,	DIVL,		046101,		"divsl",

0,	BR,		060000,		"bra",
0,	BR,		060400,		"bsr",
0,	BR,		061000,		"bhi",
0,	BR,		061400,		"bls",
0,	BR,		062000,		"bcc",
0,	BR,		062400,		"bcs",
0,	BR,		063000,		"bne",
0,	BR,		063400,		"beq",
0,	BR,		064000,		"bvc",
0,	BR,		064400,		"bvs",
0,	BR,		065000,		"bpl",
0,	BR,		065400,		"bmi",
0,	BR,		066000,		"bge",
0,	BR,		066400,		"blt",
0,	BR,		067000,		"bgt",
0,	BR,		067400,		"ble",

0,	DBR,		050310,		"dbt",
0,	DBR,		050710,		"dbf",
0,	DBR,		050710,		"dbra",
0,	DBR,		051310,		"dbhi",
0,	DBR,		051710,		"dbls",
0,	DBR,		052310,		"dbcc",
0,	DBR,		052710,		"dbcs",
0,	DBR,		053310,		"dbne",
0,	DBR,		053710,		"dbeq",
0,	DBR,		054310,		"dbvc",
0,	DBR,		054710,		"dbvs",
0,	DBR,		055310,		"dbpl",
0,	DBR,		055710,		"dbmi",
0,	DBR,		056310,		"dbge",
0,	DBR,		056710,		"dblt",
0,	DBR,		057310,		"dbgt",
0,	DBR,		057710,		"dble",

0,	OP_EXT,		044000,		"ext",
0,	OP_EXT,		044400,		"extb",

0,	OP_RANGE,	04000,		"chk2",
0,	OP_RANGE,	0,		"cmp2",

0,	TRAPCC,		050370,		"trapt",
0,	TRAPCC,		050770,		"trapf",
0,	TRAPCC,		051370,		"traphi",
0,	TRAPCC,		051770,		"trapls",
0,	TRAPCC,		052370,		"trapcc",
0,	TRAPCC,		052770,		"trapcs",
0,	TRAPCC,		053370,		"trapeq",
0,	TRAPCC,		053770,		"trapvc",
0,	TRAPCC,		054370,		"trapvs",
0,	TRAPCC,		054770,		"trappl",
0,	TRAPCC,		055370,		"trapmi",
0,	TRAPCC,		055770,		"trapge",
0,	TRAPCC,		056370,		"traplt",
0,	TRAPCC,		056770,		"trapgt",
0,	TRAPCC,		057370,		"traple",
0,	TRAPCC,		057770,		"trapne",

0,	CMP,		0,		"cmp",

0,	MOVE,		0,		"move",
0,	MOVEP,		0,		"movep",
0,	MOVEM,		0,		"movem",
0,	MOVES,		0,		"moves",
0,	MOVEC,		0,		"movec",

0,	SWAP,		0,		"swap",

0,	LINK,		0,		"link",

0,	UNLK,		0,		"unlk",

0,	TRAP,		0,		"trap",

0,	STOP,		0,		"stop",

0,	CHK,		0,		"chk",

0,	RTM,		0,		"rtm",

0,	EXG,		0,		"exg",

0,	LEA,		0,		"lea",

0,	RTD,		0,		"rtd",

0,	BKPT,		0,		"bkpt",

0,	CALLM,		0,		"callm",

0,	CAS,		0,		"cas",

0,	CAS2,		0,		"cas2",

0,	CP,		00000,		"c0",
0,	CP,		01000,		"c1",
0,	CP,		02000,		"c2",
0,	CP,		03000,		"c3",
0,	CP,		04000,		"c4",
0,	CP,		05000,		"c5",
0,	CP,		06000,		"c6",
0,	CP,		07000,		"c7",

0,	CPGEN,		0170000,	".gen",
0,	CPSCC,		0170100,	".s",
0,	CPDBCC,		0170110,	".db",
0,	CPTRAPCC,	0170170,	".trap",
0,	CPBCC,		0170200,	".br", /* 'r' to distinguish from SIZE_B
					        */
0,	CPSAVE,		0170400,	".save",
0,	CPRESTORE,	0170500,	".restore",
