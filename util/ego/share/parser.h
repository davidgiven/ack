/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
bool parse(line_p l, offset nbytes, line_p *l_out, int level,
	   void (*action0)(line_p l1, line_p l2, offset size));
			/*
			 * This is a recursive descent parser for
			 * EM expressions.
			 * It tries to recognize EM code that loads exactly
			 * 'nbytes' bytes on the stack.
			 * 'l' is the last instruction of this code.
			 * On toplevel, when an expression has been
			 * recognized, the procedure-parameter
			 * 'action0' is called, with parameters: the first and
			 * last instruction of the expression and the number of
			 * bytes recognized.
			 */
