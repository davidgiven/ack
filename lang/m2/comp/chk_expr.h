/* E X P R E S S I O N   C H E C K I N G */

extern int	(*ExprChkTable[])();	/* table of expression checking
					   functions, indexed by node class
					*/
extern int	(*DesigChkTable[])();	/* table of designator checking
					   functions, indexed by node class
					*/

#define	ChkExpression(expp)	((*ExprChkTable[(expp)->nd_class])(expp))
#define ChkDesignator(expp)	((*DesigChkTable[(expp)->nd_class])(expp))
