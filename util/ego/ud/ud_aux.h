/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* C O P Y    P R O P A G A T I O N
 *
 * A U X I L I A R Y   R O U T I N E S
 */


void repl_line(line_p old, line_p new, bblock_p b);
				/*
				 * Replace EM instruction 'old' by a
				 * copy of 'new'. Update doubly-linked
				 * list.
				 */
bool same_var(line_p use, line_p def);
				/*
				 * 'use' is an instruction that uses a variable
				 * for which we maintain ud-info (e.g. a LOL).
				 * See if 'def' references the same variable.
				 */
