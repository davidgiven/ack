/* $Id$ */

#include <alloc.h>

/* Implementation of AVL-trees: trees in which the difference in depth
   of the left branch and the right branch is at most one.
   The difference in depth is indicated by a "balance" flag in each node:
   this flag has one of three values:
   .	indicating that the left branch has the same depth as the right branch,
   +	indicating that the right branch is deeper,
   -	indicating that the left branch is deeper.
   So, a node has the following structure:
*/

struct avl_node {
  struct avl_node
		*left,
		*right;		/* the left and right branches */
  char		*info;		/* pointer to information in this node */
  char		balance;	/* balance information described above */
};

/* create definitions for new_avl_node() and free_avl_node() */
/* STATICALLOCDEF "avl_node" 10 */

/* There is also a tree header, which contains the root of the tree and
   the address of a user-supplied comparison routine:
*/

struct avl_tree {
  struct avl_node
		*root;		/* root of the avl tree */
  int		(*cmp)();	/* address of comparison routine */
};
/* create definitions for new_avl_tree() and free_avl_tree() */
/* STATICALLOCDEF "avl_tree" 2 */

/* The next routine adds a node to an avl tree. It returns 1 if the
   tree got deeper.
*/
static int
balance_add(ppsc, n, cmp)
  struct avl_node **ppsc;	/* address of root */
  register char *n;		/* user-supplied information */
  int (*cmp)();			/* user-supplied comparison routine */
{
  register struct avl_node *psc = *ppsc, *qsc, *ssc;

  if (! psc) {
	*ppsc = new_avl_node();
	(*ppsc)->balance = '.';
	(*ppsc)->info = n;
	return 1;
  }
  if ((*cmp)(n, psc->info) < 0) {
	if (balance_add(&(psc->left), n, cmp)) {
		/* left hand side got deeper */
		if (psc->balance == '+') {
			/* but the right hand side was deeper */
			psc->balance = '.';
			return 0;
		}
		if (psc->balance == '.') {
			/* but the right hand side was as deep */
			psc->balance = '-';
			return 1;
		}
		/* left hand side already was one deeper; re-organize */
		qsc = psc->left;
		if (qsc->balance == '-') {
			/* if left-hand side of left node was deeper,
			   this node becomes the new root
			*/
			psc->balance = '.';
			qsc->balance = '.';
			psc->left = qsc->right;
			qsc->right = psc;
			*ppsc = qsc;
			return 0;
		}
		/* else the right node of the left node becomes the new root */
		ssc = qsc->right;
		psc->left = ssc->right;
		qsc->right = ssc->left;
		ssc->left = qsc;
		ssc->right = psc;
		*ppsc = ssc;
		if (ssc->balance == '.') {
			psc->balance = '.';
			qsc->balance = '.';
			return 0;
		}
		if (ssc->balance == '-') {
			psc->balance = '+';
			qsc->balance = '.';
			ssc->balance = '.';
			return 0;
		}
		psc->balance = '.';
		qsc->balance = '-';
	}
	return 0;
  }
  if (balance_add(&(psc->right), n, cmp)) {
	/* right hand side got deeper */
	if (psc->balance == '-') {
		/* but the left hand side was deeper */
		psc->balance = '.';
		return 0;
	}
	if (psc->balance == '.') {
		/* but the left hand side as deep */
		psc->balance = '+';
		return 1;
	}
	/* right hand side already was one deeper; re-organize */
	qsc = psc->right;
	if (qsc->balance == '+') {
		/* if right-hand side of left node was deeper,
		   this node becomes the new root
		*/
		psc->balance = '.';
		qsc->balance = '.';
		psc->right = qsc->left;
		qsc->left = psc;
		*ppsc = qsc;
		return 0;
	}
	/* else the left node of the right node becomes the new root */
	ssc = qsc->left;
	psc->right = ssc->left;
	qsc->left = ssc->right;
	ssc->right = qsc;
	ssc->left = psc;
	*ppsc = ssc;
	if (ssc->balance == '.') {
		psc->balance = '.';
		qsc->balance = '.';
		return 0;
	}
	if (ssc->balance == '+') {
		psc->balance = '-';
		qsc->balance = '.';
		ssc->balance = '.';
		return 0;
	}
	psc->balance = '.';
	qsc->balance = '+';
  }
  return 0;
}

/* extern struct avl_tree *create_avl_tree(int (*cmp)());
   Returns a fresh avl_tree structure.
*/
struct avl_tree *
create_avl_tree(cmp)
  int	(*cmp)();		/* comparison routine */
{
  register struct avl_tree *p = new_avl_tree();

  p->cmp = cmp;
  return p;
}

/* extern add_to_avl_tree(struct avl_tree *tree, char *n);
   Adds the information indicated by 'n' to the avl_tree indicated by 'tree'
*/
add_to_avl_tree(tree, n)
  struct avl_tree	*tree;	/* tree to be added to */
  char			*n;	/* information */
{
  (void) balance_add(&(tree->root), n, tree->cmp);
}

/* extern char *find_ngt(struct avl_tree *tree, char *n);
   Returns the information in the largest node that still compares <= to 'n',
   or 0 if not present.
*/
char *
find_ngt(tree, n)
  struct avl_tree	*tree;	/* tree to be searched in */
  char			*n;	/* information to be compared with */
{
  register struct avl_node *nd = tree->root, *lastnd = 0;

  for (;;) {
  	while (nd && (*tree->cmp)(nd->info, n) > 0) {
		nd = nd->left;
	}
  	while (nd && (*tree->cmp)(nd->info, n) <= 0) {
		lastnd = nd;
		nd = nd->right;
	}
	if (! nd) break;
  }
  return lastnd ? lastnd->info : (char *) 0;
}

/* extern char *find_nlt(struct avl_tree *tree, char *n);
   Returns the information in the largest node that still compares >= to 'n',
   or 0 if not present.
*/
char *
find_nlt(tree, n)
  struct avl_tree	*tree;	/* tree to be searched in */
  char			*n;	/* information to be compared with */
{
  register struct avl_node *nd = tree->root, *lastnd = 0;

  for (;;) {
  	while (nd && (*tree->cmp)(nd->info, n) < 0) {
		nd = nd->right;
	}
  	while (nd && (*tree->cmp)(nd->info, n) >= 0) {
		lastnd = nd;
		nd = nd->left;
	}
	if (! nd) break;
  }
  return lastnd ? lastnd->info : (char *) 0;
}

/* extern char *find_eq(struct avl_tree *tree, char *n);
   Returns the information in the node that compares equal to 'n',
   or 0 if not present.
*/
char *
find_eq(tree, n)
  struct avl_tree	*tree;	/* tree to be searched in */
  char			*n;	/* information to be compared with */
{
  register struct avl_node *nd = tree->root;

  for (;;) {
  	while (nd && (*tree->cmp)(nd->info, n) < 0) {
		nd = nd->right;
	}
  	while (nd && (*tree->cmp)(nd->info, n) > 0) {
		nd = nd->left;
	}
	if (! nd) break;
  }
  return nd ? nd->info : (char *) 0;
}
