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
#include	"main.h"

static int	nsubnodes[] = {
	0,
	2,
	2,
	2,
	2,
	2,
	1,
	1,
	2,
	1,
	2,
	1,
	2
};

t_node *
getnode(class)
{
	register t_node *nd = new_node();

	if (options['R']) nd->nd_flags |= ROPTION;
	if (options['A']) nd->nd_flags |= AOPTION;
	nd->nd_class = class;
	return nd;
}

t_node *
MkNode(class, left, right, token)
	t_node *left, *right;
	t_token *token;
{
	/*	Create a node and initialize it with the given parameters
	*/
	register t_node *nd = getnode(class);

	nd->nd_token = *token;
	nd->nd_LEFT = left;
	nd->nd_RIGHT = right;
	return nd;
}

t_node *
dot2node(class, left, right)
	t_node *left, *right;
{
	return MkNode(class, left, right, &dot);
}

t_node *
MkLeaf(class, token)
	t_token *token;
{
	register t_node *nd = getnode(class);
	nd->nd_token = *token;
	switch(nsubnodes[class]) {
	case 1:
		nd->nd_NEXT = 0;
		break;
	case 2:
		nd->nd_LEFT = 0;
		nd->nd_RIGHT = 0;
		break;
	}
	return nd;
}

t_node *
dot2leaf(class)
{
	return MkLeaf(class, &dot);
}

FreeLR(nd)
	register t_node *nd;
{
	switch(nsubnodes[nd->nd_class]) {
	case 2:
		FreeNode(nd->nd_LEFT);
		FreeNode(nd->nd_RIGHT);
		nd->nd_LEFT = nd->nd_RIGHT = 0;
		break;
	case 1:
		FreeNode(nd->nd_NEXT);
		nd->nd_NEXT = 0;
		break;
	}
}

FreeNode(nd)
	register t_node *nd;
{
	/*	Put nodes that are no longer needed back onto the free
		list
	*/
	if (!nd) return;
	FreeLR(nd);
	free_node(nd);
}

NodeCrash(expp)
	t_node *expp;
{
	crash("Illegal node %d", expp->nd_class);
}

PNodeCrash(expp)
	t_node **expp;
{
	crash("Illegal node %d", (*expp)->nd_class);
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
	register t_node *nd;
{
	indnt(lvl);
	print("Class: %d; Symbol: %s; Flags: %d\n", nd->nd_class, symbol2str(nd->nd_symb), nd->nd_flags);
	if (nd->nd_type) {
		indnt(lvl);
		print("Type: ");
		DumpType(nd->nd_type);
		print("\n");
	}
}

PrNode(nd, lvl)
	register t_node *nd;
{
	if (! nd) {
		indnt(lvl); print("<nilnode>\n");
		return;
	}
	printnode(nd, lvl);
	switch(nsubnodes[nd->nd_class]) {
	case 1:
		PrNode(nd->nd_LEFT, lvl + 1);
		PrNode(nd->nd_RIGHT, lvl + 1);
		break;
	case 2:
		PrNode(nd->nd_NEXT, lvl + 1);
		break;
	}
}
#endif DEBUG
