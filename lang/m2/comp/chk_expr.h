/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
#ifndef CHK_EXPR_H_
#define CHK_EXPR_H_

/* E X P R E S S I O N   C H E C K I N G */

/* $Id$ */

extern int	(*ExprChkTable[])(t_node **, int);	/* table of expression checking
					   functions, indexed by node class
					*/
extern int	(*DesigChkTable[])(t_node **, int);	/* table of designator checking
					   functions, indexed by node class
					*/

#define	ChkExpression(expp)	((*ExprChkTable[(unsigned int)((*expp)->nd_class)])(expp,D_USED))
#define ChkDesig(expp, flags)	((*DesigChkTable[(unsigned int)((*expp)->nd_class)])(expp,flags))

/* handle reference counts for sets */
#define inc_refcount(s)		(*((int *)(s) - 1) += 1)
#define dec_refcount(s)		(*((int *)(s) - 1) -= 1)
#define refcount(s)		(*((int *)(s) - 1))

void MkCoercion(t_node **pnd, register t_type *tp);
int ChkVariable(register t_node **expp, int flags);
int ChkCall(t_node **expp);
void TryToString(register t_node *nd, t_type *tp);

/* Generates a set of "size" bytes and increments its reference count. */
arith *MkSet(unsigned int size);
/* Decrements the reference counter of set "s" and if reference count
 * is zero, then it frees the memory associated with the set.
 */
void FreeSet(register arith *s);

#endif /* CHK_EXPR_H_ */
