/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* C O N S T A N T S   F O R   E X P R E S S I O N   H A N D L I N G */

/* $Header$ */

#define	arith_sign	((arith) (1L << (sizeof(arith) * 8 - 1)))

extern unsigned int
	wrd_bits;	/* Number of bits in a word */
