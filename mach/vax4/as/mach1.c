/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Header$"

/*
 * VAX-11 Machine dependent C declarations
 */

/* Addressing modes */

#define REG_MODE	 5
#define REGDEF_MODE	 6
#define AI_MODE		 8
#define AI_DEF_MODE	 9
#define AD_MODE		 7
#define DISPLB_MODE	10
#define DISPLW_MODE	12
#define DISPLL_MODE	14
#define DISPLB_DEF_MODE	11
#define DISPLW_DEF_MODE	13
#define DISPLL_DEF_MODE	15
#define INDEX_MODE	 4

#define DISPL		16	/* not an addressing mode; used for branch
				   displacement addressing
				*/
#define IMM		17	/* immediate mode (only for internal use) */
#define ABS		18	/* absolute mode (only for internal use) */
#define ABS_DEF		19	/* absolute deferred mode (only for internal use) */

#define PC		15	/* special case */

#define fit8(z)		(lowb(z) == (z))
#define fit16(z)	(loww(z) == (z))
#define literal(z)	(((z) & ~0x3f) == 0)

struct operand {
	expr_t	exp;
	int	mode;		/* addressing mode */
	int	reg;		/* register used in addressing mode */
	int	relo;		/* index in relocation table for exp */
	int	size;		/* size as imposed by instruction */
	int	index_reg;	/* for indexed mode contains index reg,
				   -1 if not index mode
				*/
};

extern struct operand opnd[6];
extern int op_ind;
