/* E X P R E S S I O N   C H E C K I N G */

/* $Header$ */

extern int	(*ExprChkTable[])();	/* table of expression checking
					   functions, indexed by node class
					*/
extern int	(*DesigChkTable[])();	/* table of designator checking
					   functions, indexed by node class
					*/

#define	chk_expr(expp)	((*ExprChkTable[(expp)->nd_class])(expp))
#define chk_designator(expp)	((*DesigChkTable[(expp)->nd_class])(expp))
