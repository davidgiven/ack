/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

typedef struct expr {
	int	ex_typ;
	short	ex_regset[SZOFSET(MAXREGS)];
	int	ex_index;
} expr_t,*expr_p;

#define TYPINT  1
#define TYPBOOL 2
#define TYPADDR 3
#define TYPREG  4

/* When the type is register the regset contains the set of
   possible registers for checking purposes only.
*/
