/* E X P R E S S I O N   C H E C K I N G */

/* $Header$ */

extern int	(*ChkTable[])();	/* table of expression checking
					   functions, indexed by node class
					*/

#define	chk_expr(expp)	((*ChkTable[(expp)->nd_class])(expp))
