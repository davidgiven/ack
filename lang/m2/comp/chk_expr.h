/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* E X P R E S S I O N   C H E C K I N G */

/* $Id$ */

extern int	(*ExprChkTable[])();	/* table of expression checking
					   functions, indexed by node class
					*/
extern int	(*DesigChkTable[])();	/* table of designator checking
					   functions, indexed by node class
					*/

#define	ChkExpression(expp)	((*ExprChkTable[(*expp)->nd_class])(expp,D_USED))
#define ChkDesig(expp, flags)	((*DesigChkTable[(*expp)->nd_class])(expp,flags))

/* handle reference counts for sets */
#define inc_refcount(s)		(*((int *)(s) - 1) += 1)
#define dec_refcount(s)		(*((int *)(s) - 1) -= 1)
#define refcount(s)		(*((int *)(s) - 1))
