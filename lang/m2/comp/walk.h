/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* P A R S E   T R E E   W A L K E R */
#ifndef WALK_H_
#define WALK_H_

#include "em_label.h"

/* Forward type declarations. */
struct node;
struct def;
struct desig;

/*	Definition of WalkNode macro
*/
extern int (*WalkTable[])(struct node*, label, int);

#define	WalkNode(xnd, xlab, rch)	(*WalkTable[(unsigned int)((xnd)->nd_class)])((xnd), (xlab),(rch))

extern label	text_label;
extern label	data_label;

#ifndef SQUEEZE
#define c_loc(x)	C_loc((arith) (x))
#define c_lae_dlb(x)	C_lae_dlb(x,(arith) 0)
#define CAL(nm, sz)	(C_cal(nm), C_asp((arith)(sz)))
#define c_bra(x)	C_bra((label) (x))
#endif



int LblWalkNode(label lbl, struct node *nd, int exit, int reach);
void def_ilb(label l);
/* Generate line information as necessary for "nd". */
void DoLineno(register struct node* nd);
/*	Generate filename information, when needed.
	This routine is called at the generation of a
	procedure entry, and after generating a call to
	another procedure. The current active filename
	is used.
*/
void DoFilename(int needed);

/*	Walk through a module, and all its local definitions.
	Also generate code for its body.
	This code is collected in an initialization routine.
*/
void WalkModule(register struct def* module);
/*	Walk through the definition of a procedure and all its
	local definitions, checking and generating code.
*/
void WalkProcedure(register struct def* procedure);

/*	Walk node "nd", which is a link.
	"exit_label" is set to a label number when inside a LOOP.
	"end_reached" maintains info about reachability (REACH_FLAG),
	and whether an EXIT statement was seen (EXIT_FLAG).
*/
int WalkLink(register struct node* nd, label exit_label, int end_reached);
/* Walk through a statement node "nd", generating code for it. */
int WalkStat(register struct node* nd, label exit_label, int end_reached);
/*	Generate code to evaluate a boolean expression "pnd" */
void ExpectBool(register struct node** pnd, label true_label, label false_label);
/* Check designator and generate code for it */
int WalkDesignator(struct node** pnd, struct desig* ds, int flags);

void DoAssign(register struct node* nd);

int DoForInit(struct node* nd);

#endif /* WALK_H_ */
