/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdio.h>
#include <em_spec.h>
#include <em_mnem.h>
#include "types.h"
#include "debug.h"
#include "alloc.h"
#include "global.h"
#include "lset.h"
#include "aux.h"

struct class {
	byte	src_class;
	byte	res_class;
};

typedef struct class *class_p;


#define NOCLASS	0
#define CLASS1	1
#define CLASS2	2
#define CLASS3	3
#define CLASS4	4
#define CLASS5	5
#define CLASS6	6
#define CLASS7	7
#define CLASS8	8
#define CLASS9	9
#define CLASS10 10
#define CLASS11	11
#define CLASS12	12

#include "classdefs.h"
/* The file classdefs.h contains the table classtab. It is
 * generated automatically from the file classdefs.src.
 */

STATIC bool classes(instr,src_out,res_out)
	int instr;
	int *src_out, *res_out;
{
	/* Determine the classes of the given instruction */

	class_p c;

	if (instr < sp_fmnem || instr > sp_lmnem) return FALSE;
	c = &classtab[instr];
	if (c->src_class == NOCLASS) return FALSE;
	*src_out = c->src_class;
	*res_out = c->res_class;
	return TRUE;
}



STATIC bool uses_arg(class)
	int class;
{
	/* See if a member of the given class uses
	 * an argument.
	 */

	switch(class) {
		case CLASS1:
		case CLASS2:
		case CLASS3:
		case CLASS4:
		case CLASS11:
		case CLASS12:
			return TRUE;
		default:
			return FALSE;
	}
	/* NOTREACHED */
}



STATIC bool uses_2args(class)
	int class;
{
	/* See if a member of the given class uses
	 * 2 arguments.
	 */

	return class == CLASS10;
}


STATIC bool parse_locs(l,c1_out,c2_out)
	line_p l;
	offset *c1_out, *c2_out;
{
	if (INSTR(l) == op_loc && INSTR(PREV(l)) == op_loc) {
		*c1_out = off_set(l);
		*c2_out = off_set(PREV(l));
		return TRUE;
	}
	return FALSE;
}



STATIC bool check_args(l,src_class,res_class,arg1_out,arg2_out)
	line_p l;
	int    src_class,res_class;
	offset *arg1_out, *arg2_out;
{
	/* Several EM instructions have an argument
	 * giving the size of the operand(s) of
	 * the instruction. E.g. a 'adi 4' is a 4-byte
	 * addition. The size may also be put on the
	 * stack. In this case we give up our
	 * efforts to recognize the parameter expression.
	 * Some instructions (e.g. CIU) use 2 arguments
	 * that are both on the stack. In this case we
	 * check if both arguments are LOCs (the usual case),
	 * else we give up.
	 */

	if (uses_2args(src_class) || uses_2args(res_class)) {
		return parse_locs(PREV(l),arg1_out,arg2_out);
	}
	if (uses_arg(src_class) || uses_arg(res_class)) {
		if (TYPE(l) == OPSHORT) {
			*arg1_out = (offset) SHORT(l);
			return TRUE;
		} else {
			if (TYPE(l) == OPOFFSET) {
				*arg1_out = OFFSET(l);
			} else {
				return FALSE;
			}
		}
	}
	return TRUE; /* no argument needed */
}



STATIC offset nrbytes(class,arg1,arg2)
	int class;
	offset arg1,arg2;
{
	/* Determine the number of bytes of the given
	 * arguments and class.
	 */

	switch(class) {
		case CLASS1:
			return arg1;
		case CLASS2:
			return 2 * arg1;
		case CLASS3:
			return arg1 + ws;
		case CLASS4:
			return arg1 + ps;
		case CLASS5:
			return ws;
		case CLASS6:
			return 2 * ws;
		case CLASS7:
			return ps;
		case CLASS8:
			return 2 * ps;
		case CLASS9:
			return 0;
		case CLASS10:
			return arg2 + 2*ws;
		case CLASS11:
			return arg1 + 2*ps;
		case CLASS12:
			return (arg1 < ws ? ws : arg1);
		default:
			assert(FALSE);
	}
	return 0;
}



STATIC attrib(l,expect_out,srcb_out,resb_out)
	line_p l;
	offset    *expect_out, *srcb_out, *resb_out;
{
	/* Determine a number of attributes of an EM
	 * instruction appearing in an expression.
	 * If it is something we don't
	 * expect in such expression (e.g. a store)
	 * expect_out is set to FALSE. Else we
	 * determine the number of bytes popped from
	 * the stack by the instruction and the
	 * number of bytes pushed on the stack as
	 * result.
	 */

	int src_class,res_class;
	offset arg1, arg2;

	if (l == (line_p) 0 || !classes(INSTR(l),&src_class,&res_class) ||
	    !check_args(l,src_class,res_class,&arg1,&arg2)) {
		*expect_out = FALSE;
	} else {
		*expect_out = TRUE;
		*srcb_out = nrbytes(src_class,arg1,arg2);
		*resb_out = nrbytes(res_class,arg1,arg2);
	}
}



bool parse(l,nbytes,l_out,level,action0)
	line_p l, *l_out;
	offset nbytes;
	int    level;
	int    (*action0) ();
{
	/* This is a recursive descent parser for
	 * EM expressions.
	 * It tries to recognize EM code that loads exactly
	 * 'nbytes' bytes on the stack.
	 * 'l' is the last instruction of this code.
	 * As EM is essentially postfix, this instruction
	 * can be regarded as the root node of an expression
	 * tree. The EM code is traversed from right to left,
	 * i.e. top down. On success, TRUE is returned and
	 * 'l_out' will point to the first instruction
	 * of the recognized code. On toplevel, when an
	 * expression has been recognized, the procedure-parameter
	 * 'action0' is called, with parameters: the first and
	 * last instruction of the expression and the number of
	 * bytes recognized.
	 */

	offset more, expected, sourcebytes,resultbytes;
	line_p lnp = 0;

	more = nbytes; /* #bytes to be recognized */
	while (more > 0) {
		attrib(l,&expected,&sourcebytes,&resultbytes);
		/* Get the attributes of EM instruction 'l'.
		 * 'expected' denotes if it is something we can use;
		 * 'sourcebytes' and 'resultbytes' are the number of
		 * bytes popped resp. pushed by the instruction
		 * (e.g. 'adi 2' pops 4 bytes and pushes 2 bytes).
		 */
		if (!expected || (more -= resultbytes) < 0) return FALSE;
		if (sourcebytes == 0) {
			/* a leaf of the expression tree */
			lnp = l;
		} else {
			if (!parse(PREV(l),sourcebytes,&lnp,level+1,action0)) {
				return FALSE;
			}
		}
		if (level == 0) {
			/* at toplevel */
			(*action0) (lnp,l,resultbytes);
		}
		l = PREV(lnp);
	}
	/* Now we've recognized a number of expressions that
	 * together push nbytes on the stack.
	 */
	*l_out = lnp;
	return TRUE;
}
