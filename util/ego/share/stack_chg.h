/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* S T A C K _ C H A N G E . H */

extern line_change();	/* ( line_p l; bool *ok_out; int *pop_out,*push_out)
			 * Try to determine how the stack-height will be
			 * affected by the EM instruction l. 'ok_out' is set
			 * to false if we fail to do so. pop_out and
			 * push_out are set to the number of bytes popped
			 * and pushed. E.g. for an "ADI 2" 4 and 2 are returned.
			 */
