/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  G L O B A L   M A C R O   D E F I N I T I O N S
 *
 *  F O R   A L L   O P T I M I Z E R   P A S S E S
 */

#define MARK_DBLOCK	0
#define MARK_OBJ	1
#define MARK_ARG	2


#define op_lab	(sp_lmnem+1)
#define op_last	op_lab
#define ps_sym	(sp_lpseu+1)
#define ps_last ps_sym
