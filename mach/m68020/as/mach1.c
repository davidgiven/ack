/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * Motorola 68020 dependent C declarations
 */

#define	low3(z)		((short)(z) & 07)
#define	low4(z)		((short)(z) & 017)
#define	low5(z)		((short)(z) & 037)
#define	low6(z)		((short)(z) & 077)

#define	fit3(z)		((((z)-1) & ~07) == 0)
#define	fit4(z)		(((z) & ~017) == 0)
#define	fit5(z)		(((z) & ~037) == 0)
#define	fit6(z)		(((z) & ~077) == 0)
#define	fit8(z)		(((z) & ~0xFF) == 0)
#define	fit16(z)	(((z) & ~0xFFFF) == 0)

#define	SIZE_B		0000
#define	SIZE_W		0100
#define	SIZE_L		0200
#define	SIZE_NON	0300
#define	SIZE_DEF	SIZE_W

extern int	mrg_1,mrg_2;	/* Mode and ReGister */
extern short	ffew_1, ffew_2; /* Full Format Extension Word */
extern expr_t	bd_1,bd_2,od_1,od_2; /* base and outer displacements */
#ifndef ASLD
extern valu_t	bd_rel1,bd_rel2,od_rel1,od_rel2;
#endif

#ifdef RELOCATION
#ifdef ASLD
#define	T_EMIT2(a,b,c,d)	t_emit2(a,b,c,0)
#define	T_EMIT4(a,b,c,d)	t_emit4(a,b,c,0)
#else  ALSD
#define	T_EMIT2(a,b,c,d)	t_emit2(a,b,c,d)
#define	T_EMIT4(a,b,c,d)	t_emit4(a,b,c,d)
#endif ASLD
#else
#define	T_EMIT2(a,b,c,d)	t_emit2(a)
#define	T_EMIT4(a,b,c,d)	t_emit4(a)
#endif

struct instr_t {
	short	i_word;		/* word to be emitted */
#ifdef RELOCATION
	short	i_relotype;
	int	i_reloinfo;
	valu_t	i_relonami;
#endif
};
typedef struct instr_t instr_t;
instr_t instr[30];	/* temporary area for instructions */
instr_t *instrp;	/* index into instr[] */
int dot_offset;

/* flags used when reading effective addresses */
#define INDEX		0300
#define PRE_INDEX	0100
#define POST_INDEX	0200
#define DBL_INDEX	0300
#define PC_MODE		0400

/* addressing mode bits */
#define	DTA		0x01
#define	MEM		0x02
#define	CTR		0x04
#define	ALT		0x08
#define	FITB		0x10
#define	FITW		0x20
#define	PUTW		0x40
#define	PUTL		0x80

#ifndef extern
extern short	eamode[];
#else
short	eamode[] = {
/* 00A */	DTA        |ALT,
/* 01A */	            ALT,
/* 02A */	DTA|MEM|CTR|ALT,
/* 03A */	DTA|MEM    |ALT,
/* 04A */	DTA|MEM    |ALT,
/* 05A */	DTA|MEM|CTR|ALT|FITW|PUTW, /* if relocation wanted, use 06A */
/* 06A */	DTA|MEM|CTR|ALT     |PUTW, /* relocation handled by ea_1() */
/* 07x */	0,
/* 070 */	DTA|MEM|CTR|ALT|FITW|PUTW | (RELO2)<<8,
/* 071 */	DTA|MEM|CTR|ALT     |PUTL | (RELO4)<<8,
/* 072 */	DTA|MEM|CTR    |FITW|PUTW, /* if relocation wanted, use 073 */
/* 073 */	DTA|MEM|CTR         |PUTW, /* relocation handled by ea_1() */
/* 074x */	0,
/* 074B */	DTA|MEM        |FITB|PUTW | (RELO1)<<8,
/* 074W */	DTA|MEM        |FITW|PUTW | (RELO2)<<8,
/* 074L */	DTA|MEM             |PUTL | (RELO4)<<8,
};
			/* RELO2 had to go with mode 070, because the assembly
			 * language programmer can use addresses like
			 * '(expression).w', which still have to be relocatable.
			 */
#endif
