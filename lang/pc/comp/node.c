/* N O D E   O F   A N   A B S T R A C T   P A R S E T R E E */

#include    "parameters.h"
#include	"debug.h"

#include	<stdlib.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"node.h"
#include	"type.h"
#include	"error.h"

struct node *MkNode(int class, struct node *left, struct node *right, struct token *token)
{
	/*	Create a node and initialize it with the given parameters
	*/
	struct node *nd = new_node();

	nd->nd_left = left;
	nd->nd_right = right;
	nd->nd_token = *token;
	nd->nd_class = class;
	nd->nd_type = error_type;
	return nd;
}

struct node *MkLeaf(int class, struct token *token)
{
	struct node *nd = new_node();

	nd->nd_left = nd->nd_right = NULLNODE;
	nd->nd_token = *token;
	nd->nd_type = error_type;
	nd->nd_class = class;
	return nd;
}

void FreeNode(struct node *nd)
{
	/*	Put nodes that are no longer needed back onto the free list
	*/
	if( !nd ) return;
	FreeNode(nd->nd_left);
	FreeNode(nd->nd_right);
	free_node(nd);
}

int NodeCrash(struct node *expp)
{
	crash("Illegal node %d", expp->nd_class);
}

#ifdef DEBUG



void indnt(int lvl)
{
	while( lvl-- )
		printf("  ");
}

void printnode(struct node *nd, int lvl)
{
	indnt(lvl);
	printf("Class: %d; Symbol: %s\n", nd->nd_class, symbol2str(nd->nd_symb));
	if( nd->nd_type )	{
		indnt(lvl);
		printf("Type: ");
		DumpType(nd->nd_type);
		printf("\n");
	}
}

void PrNode(struct node *nd, int lvl)
{
	if( !nd )	{
		indnt(lvl); printf("<nilnode>\n");
		return;
	}
	PrNode(nd->nd_left, lvl + 1);
	printnode(nd, lvl);
	PrNode(nd->nd_right, lvl + 1);
}
#endif
