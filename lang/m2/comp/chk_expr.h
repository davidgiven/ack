/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* E X P R E S S I O N   C H E C K I N G */

/* $Header$ */

extern int	(*ExprChkTable[])();	/* table of expression checking
					   functions, indexed by node class
					*/
extern int	(*DesigChkTable[])();	/* table of designator checking
					   functions, indexed by node class
					*/

#define	ChkExpression(expp)	((*ExprChkTable[(expp)->nd_class])(expp))
#define ChkDesignator(expp)	((*DesigChkTable[(expp)->nd_class])(expp))

#define inc_refcount(s)		(*((s) - 1) += 1)
#define dec_refcount(s)		(*((s) - 1) -= 1)
#define refcount(s)		(*((s) - 1))
