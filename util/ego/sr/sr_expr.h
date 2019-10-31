/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N 
 *
 * S R _ E X P R . H
 *
 */

bool is_ivexpr(line_p l, lset ivs, lset vars, line_p *lbegin, iv_p *iv,
	       int *out_sign);
			/*
			 * Try to recognize an expression that is a linear
			 * function of presicely one induction variable.
			 * It may only use loop constants (besides the
			 * induc. var.).
			 */
