/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* Contents of flags used when describing interpreter opcodes */

#define	RCS_IP		"$Id$"

#define OPTYPE  07      /* type field in flag */

#define OPMINI  0       /* m  MINI */
#define OPSHORT 1       /* s  SHORT */
#define OPNO    2       /* -  No operand */
#define OP8     3       /* 1  1-byte signed operand */
#define OP16    4       /* 2  2-byte signed operand */
#define OP32    5       /* 4  4-byte signed operand */
#define OP64    6       /* 8  8-byte signed operand */
#define OP16U	7	/* u  2-byte unsigned operand */

#define OPESC   010     /* e  escaped opcode */
#define OPWORD  020     /* w  operand is word multiple */
#define OPNZ    040     /* o  operand starts at 1 ( or wordsize if w-flag) */

#define OPRANGE 0300    /* Range of operands: Positive, negative, both */

#define OP_BOTH 0000    /* the default */
#define OP_POS  0100    /* p  Positive (>=0) operands only */
#define OP_NEG  0200    /* n  Negative (<0) operands only */

struct opform {
	char    i_opcode ;      /* the opcode number */
	char    i_flag   ;      /* the flag byte */
	char    i_low    ;      /* the interpreter first opcode */
	char    i_num    ;      /* the number of shorts/minis (optional) */
};

/* Escape indicators */

#define ESC     254             /* To escape group */
#define ESC_L   255             /* To 32 and 64 bit operands */
