/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
extern	Push();		/* (token_p tkp)
			 * Push the token in tkp on the fake-stack.
			 */

extern	Pop();		/* (token_p tkp; offset size)
			 * Pop a token of size bytes from the fake-stack
			 * into tkp. If such a token is not there
			 * we put a dummy in tkp and adjust the fake-stack.
			 */

extern	Dup();		/* (line_p lnp)
			 * Reflect the changes made by the dup-instruction
			 * in lnp to the EM-stack into the fake-stack.
			 */

extern	clr_stack();	/* ()
			 * Clear the fake-stack.
			 */
