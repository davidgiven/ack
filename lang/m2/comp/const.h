/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* C O N S T A N T S   F O R   E X P R E S S I O N   H A N D L I N G */

/* $Header$ */

extern long
	mach_long_sign;	/* sign bit of the machine long */
extern int
	mach_long_size;	/* size of long on this machine == sizeof(long) */
extern arith
	max_int,	/* maximum integer on target machine	*/
	max_unsigned, 	/* maximum unsigned on target machine	*/
	max_longint,	/* maximum longint on target machine	*/
	wrd_bits;	/* Number of bits in a word */
