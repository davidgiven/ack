/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * mes ms_reg,offset,size,type,priority
 *
 * Here are the defines for type
 */

#define reg_any		0	/* Unspecified type */
#define reg_loop	1	/* loop control variable */
#define reg_pointer	2	/* pointer variable */
#define reg_float	3	/* floating point variable */
