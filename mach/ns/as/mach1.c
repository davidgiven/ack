/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Header$"

/*
 * NS 16032 C declarations
 */

#define low4(val)	((int)(val&0xF))

/* signed value fits in ? bits */
#define fit32(val)	( (val)>= -2147483647L-1 && (val)<=2147483647L )
#define fit16(val)	( (val)>= -32767-1 && (val)<=32767 )
#define fit8(val)	( (val)>= -128 && (val)<=127 )
#define	fit4(val)	( (val)>= -8 && (val)<=7 )

/* Fits in byte, word displacement */
#define	fitd_b(val)	( (val)>= -64 && (val)<=63 )
#define	fitd_w(val)	( (val)>= -8192 && (val)<=8191 )

/* Assemble and disassemble operator id's */
/* the i_value's in the instruction keyword table
   contain five bit fields:
	0-3	Opcode used in format indicated by the i_type field
	4-7	Condition Code field. B_?? Also used for reg in FFS type.
	8-11	The type of the first addressing mode gen1
	12-15	The type of the second addressing mode gen2
			The integer type T_????
			The custom Slave type S_????
			The Floating point type F_????
*/

#define MK_op(op)	(op)
#define MK_cc(cc)	( (cc)<<4 )
#define MK_g1(g1)	( (g1)<<8 )
#define MK_g2(g2)	( (g2)<<12 )

#define mk_op(o)	(MK_op(o)|MK_g1(X_ILLGL)|MK_g2(X_ILLGL))
#define mk_op1(o,g1)	(MK_op(o)|MK_g1(g1)|MK_g2(X_ILLGL))
#define mk_op2(o,g1,g2) (MK_op(o)|MK_g1(g1)|MK_g2(g2))
#define mk_c(c)		MK_cc(c)
#define mk_op1c(o,g1,c)	(MK_op(o)|MK_g1(g1)|MK_g2(X_ILLGL)|MK_cc(c))
#define mk_op2c(o,g1,g2,c)	(MK_op(o)|MK_g1(g1)|MK_g2(g2)|MK_cc(c))

#define id_op(id)	((id)      &0xF)
#define id_cc(id)	(((id)>>4) &0xF)
#define id_g1(id)	(((id)>>8) &3)
#define id_g2(id)	(((id)>>12)&3)
#define id_t1(id)	(((id)>>10)&3)
#define id_t2(id)	(((id)>>14)&3)

/* Type definitions */
#define T_INT		0
#define T_FL		1
#define T_SLAVE		2
#define T_ILLGL		3

#define	I_BYTE		(T_INT<<2) | 0
#define I_WORD		(T_INT<<2) | 1
#define I_DOUBLE	(T_INT<<2) | 3

#define F_FLOAT		(T_FL<<2) | 1
#define F_LONG		(T_FL<<2) | 0

#define S_DOUBLE	(T_SLAVE<<2) | 1
#define S_QUAD		(T_SLAVE<<2) | 0

#define X_ILLGL		(T_ILLGL<<2) | 2

#define B_EQ		0x0
#define B_NE		0x1
#define B_CS		0x2
#define B_CC		0x3
#define B_HI		0x4
#define B_LS		0x5
#define B_GT		0x6
#define B_LE		0x7
#define B_FS		0x8
#define B_FC		0x9
#define B_LO		0xA
#define B_HS		0xB
#define B_LT		0xC
#define B_GE		0xD
#define B_TRUE		0xE
#define B_FALSE		0xF

/* String option bits */
#define SO_UNTIL	0xC
#define SO_WHILE	0x4
#define SO_BACKW	0x2
#define SO_TRANS	0x1

/* output definitions */
#define	form0(id)	emit1(0xA + (id_cc(id)<<4) )
#define	form1(id)	emit1(0x2 + (id_op(id)<<4) )
#define form6(id)	emit1(0x4E) ; form4(id)
#define form7(id)	emit1(0xCE) ; form4(id)


/* Structure contains information for generation of an addressing mode */
/* The size of immediates is not stored in here and must be fetched from
   a id-field
*/
extern struct {
	int	m_mode;		/* The main mode bits */
	int	m_index ;	/* m_mode= index mode, m_index has index byte */
	int	m_ndisp ;	/* The number of displacements */
	expr_t	m_expr1 ;	/* The first expression */
#ifdef RELOCATION
	int	m_rel1, m_rel2;	/* relocation */
#endif
	expr_t	m_expr2 ;	/* The second expression */
} mode1, mode2, *mode_ptr ;

#define	not_imm(mode_ptr)	if ( (mode_ptr)->m_mode==0x14 ) ill_imm() ;
