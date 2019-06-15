/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

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
#define ABS		17	/* absolute mode (only for internal use) */
#define IMM		18	/* immediate mode (only for internal use) */
#define REL		19	/* relative mode (only for internal use) */
#define REL_DEF		20	/* relative deferred mode (only for internal
				   use)
				*/

/* Some mnemonics used in mach5.c */

#define PC		15	/* program counter */
#define BRB		0x11	/* opcode of branch with byte offset */
#define BRW		0x31	/* opcode of branch with word offset */
#define JMP		0x17	/* opcode of jmp instruction */

/* Test if a constant is suitable for literal mode */

#define literal(z)	(((z) & ~(int)0x3f) == 0)

/* Declarations for operands */

struct operand {
	expr_t	exp;
	int	mode;		/* addressing mode */
	int	reg;		/* register used in addressing mode */
	int	relo;		/* index in relocation table for exp */
	int	size;		/* size as imposed by instruction
				   -1 indicates bit-field (no immediate mode
				      allowed)
				   -2 indicates address (no register mode
				      or immediate mode allowed)
				*/
	int	index_reg;	/* for indexed mode contains index reg,
				   -1 if not index mode
				*/
};

extern struct operand opnd[6];	/* maximum number of operands of a VAX
				   instruction is 6
				*/
extern int op_ind;		/* index in opng array */

/* VAX4 Specific instructions */
void branch(int opc, expr_t exp);
void ext_branch(int opc, expr_t exp);
void operands(void);

