/* $Header; mach0.c, v1.2 06-Mar-89 AJM */

#include <stdint.h>

#define THREE_PASS
#define LISTING     /* enable listing facilities */
#define RELOCATION  /* generate relocation info */
/*
#define WORDS_REVERSED
#define BYTES_REVERSED
*/
#undef valu_t
#define valu_t 	int32_t

#undef ADDR_T
#define ADDR_T uint32_t

#undef word_t
#define word_t	uint32_t

#undef ALIGNWORD
#define ALIGNWORD	4

#undef ALIGNSECT
#define ALIGNSECT	4

#undef VALWIDTH
#define VALWIDTH	8

#define S_REG		0xF
#define S_NUM		0x8

#define	ADC	0x00A00000
#define	ADD	0x00800000
#define	AND	0x00000000
#define	BIC	0x01C00000
#define	EOR	0x00200000
#define	ORR	0x01800000
#define	RSB	0x00600000
#define	RSC	0x00E00000
#define	SBC	0x00C00000
#define	SUB	0x00400000
#define	MOV	0x01A00000
#define	MVN	0x01E00000
#define	CMN	0x01700000
#define	CMP	0x01500000
#define	TEQ	0x01300000
#define	TST	0x01100000
#define LDR	0x04100000
#define STR	0x04000000

