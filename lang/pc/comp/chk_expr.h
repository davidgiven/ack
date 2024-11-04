/* E X P R E S S I O N   C H E C K I N G */

struct node;

extern int	(*ExprChkTable[])(struct node*);	/* table of expression checking
					   functions, indexed by node class
					*/

extern int	(*VarAccChkTable[])(struct node*);	/* table of variable-access checking
					   functions, indexed by node class
					*/

#define	ChkExpression(expp)	((*ExprChkTable[(expp)->nd_class])(expp))
#define	ChkVarAccess(expp)	((*VarAccChkTable[(expp)->nd_class])(expp))

int ChkConstant(struct node *expp);
int ChkVariable(struct node *expp);
/* Check that "expp" indicates an item that can be the lhs
   of an assignment, return 1 if possible, on return 0.
 */
int ChkLhs(struct node *expp);
int ChkLinkOrName(struct node *expp);
char *ChkAllowedVar(struct node *nd, int reading);
int ChkCall(struct node *expp);
void MarkUsed(struct node *nd);
