/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* R E A L   C O N S T A N T   D E S C R I P T O R   D E F I N I T I O N */

/* $Id$ */

#include <flt_arith.h>

struct real {
	char		*r_real;
	flt_arith	r_val;
};

/* ALLOCDEF "real" 20 */
