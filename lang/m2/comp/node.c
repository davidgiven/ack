/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* N O D E   O F   A N   A B S T R A C T   P A R S E T R E E */

/* $Header$ */

#include	"debug.h"

#include	<em_label.h>
#include	<em_arith.h>
#include	<alloc.h>
#include	<system.h>

#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"node.h"

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
	return nd;
}

struct node *
dot2node(class, left, right)
	struct node *left, *right;
{
	return MkNode(class, left, right, &dot);
}

struct node *
MkLeaf(class, token)
	struct token *token;
{
	register struct node *nd = new_node();

	nd->nd_token = *token;
	nd->nd_class = class;
	return nd;
}

struct node *
dot2leaf(class)
{
	return MkLeaf(class, &dot);
}

FreeNode(nd)
	register struct node *nd;
{
	/*	Put nodes that are no longer needed back onto the free
		list
	*/
	if (!nd) return;
	FreeNode(nd->nd_left);
	FreeNode(nd->nd_right);
	free_node(nd);
}

NodeCrash(expp)
	struct node *expp;
{
	crash("Illegal node %d", expp->nd_class);
}

#ifdef DEBUG

extern char *symbol2str();

indnt(lvl)
{
	while (lvl--) {
		print("  ");
	}
}

printnode(nd, lvl)
	register struct node *nd;
{
	indnt(lvl);
	print("Class: %d; Symbol: %s\n", nd->nd_class, symbol2str(nd->nd_symb));
	if (nd->nd_type) {
		indnt(lvl);
		print("Type: ");
		DumpType(nd->nd_type);
		print("\n");
	}
}

PrNode(nd, lvl)
	register struct node *nd;
{
	if (! nd) {
		indnt(lvl); print("<nilnode>\n");
		return;
	}
	printnode(nd, lvl);
	PrNode(nd->nd_left, lvl + 1);
	PrNode(nd->nd_right, lvl + 1);
}
#endif DEBUG
