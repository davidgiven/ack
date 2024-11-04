/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef BODY_H_
#define BODY_H_

#include "em_arith.h"
#include "em_label.h"


struct node;
struct scopelist;


void MarkDef(struct node *nd, unsigned short flags, int on);

/* Assert statement */
void AssertStat(struct node *expp, unsigned short line);
/** Assign statement */
void AssignStat(struct node *left, struct node *right);
/** Procedure call statement */
void ProcStat(struct node *nd);

/** ??? */
void ChkForStat(struct node *nd);
/** ??? */
void EndForStat(struct node *nd);
arith CodeInitFor(struct node *nd, int priority);
void CodeFor(struct node *nd, int stepsize, label l1, label l2);
void CodeEndFor(struct node *nd, int stepsize, label l1, label l2, arith tmp2);

/* With statement */
void WithStat(struct node *nd);
void EndWith(struct scopelist *saved_scl, struct node *nd);

#endif /* BODY_H_ */
