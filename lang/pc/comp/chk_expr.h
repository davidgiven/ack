/* E X P R E S S I O N   C H E C K I N G */

struct node;

extern int	(*ExprChkTable[])();	/* table of expression checking
					   functions, indexed by node class
					*/

extern int	(*VarAccChkTable[])();	/* table of variable-access checking
					   functions, indexed by node class
					*/

#define	ChkExpression(expp)	((*ExprChkTable[(expp)->nd_class])(expp))
#define	ChkVarAccess(expp)	((*VarAccChkTable[(expp)->nd_class])(expp))

int ChkConstant(register struct node *expp);
int ChkVariable(register struct node *expp);
/* Check that "expp" indicates an item that can be the lhs
   of an assignment, return 1 if possible, on return 0.
 */
int ChkLhs(register struct node *expp);
int ChkLinkOrName(register struct node *expp);
char *ChkAllowedVar(register struct node *nd, int reading);
int ChkCall(register struct node *expp);
void MarkUsed(register struct node *nd);
