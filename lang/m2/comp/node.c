/* N O D E   O F   A N   A B S T R A C T   P A R S E T R E E */

static char *RcsId = "$Header$";

#include	<em_label.h>
#include	<em_arith.h>
#include	<alloc.h>
#include	"def.h"
#include	"type.h"
#include	"LLlex.h"
#include	"node.h"

struct node *h_node;		/* header of free list */

struct node *
MkNode(class, left, right, token)
	struct node *left, *right;
	struct token *token;
{
	/*	Create a node and initialize it with the given parameters
	*/
	register struct node *nd = new_node();

	nd->nd_left = left;
	nd->nd_right = right;
	nd->nd_token = *token;
	nd->nd_class = class;
	nd->nd_type = NULLTYPE;
	return nd;
}

FreeNode(nd)
	register struct node *nd;
{
	/*	Put nodes that are no longer needed back onto the free
		list
	*/
	if (nd->nd_left) FreeNode(nd->nd_left);
	if (nd->nd_right) FreeNode(nd->nd_right);
	free_node(nd);
}
