/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/*	Bit patterns to allow fast answers to set questions about classes.
	The classes in the inpdef are given as single letters; the array
	class[] contains a bit pattern for each letter, with those bits set
	that correspond to the lint class meaning of the letter.

	This facility is used through the macro  is_class(inpdef_var, CL_XXX)
*/

#define	CL_DEF		(1<<0)
#define	CL_DECL		(1<<1)
#define	CL_USAGE	(1<<2)

#define	CL_FUNC		(1<<3)
#define	CL_VAR		(1<<4)

#define	CL_LIB		(1<<5)
#define	CL_EXT		(1<<6)
#define	CL_IMPL		(1<<7)
#define	CL_STAT		(1<<8)

extern int class[];

#define	is_class(id,cl)	((class[(id)->id_class-MIN_CLASS_CONST] & (cl)) == (cl))

