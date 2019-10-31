/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ P R O F I T S . H
 */

void compute_profits(alloc_p alloclist, bool time_opt);
			/*
			 * Compute the profits attribute of every allocation.
			 */
void regsave_cost(short regs[], short *time_out, short *space_out);
