/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef ENTER_H_
#define ENTER_H_

#include "em_arith.h"

/* Forward structure declarations. */
struct type;
struct node;
struct paramlist;


struct def *Enter(char *name, long kind, struct type *type, int pnam);
void EnterProgList(struct node *Idlist);
void EnterEnumList(struct node *Idlist, struct type *type);
void EnterFieldList(struct node *Idlist, struct type *type,
		struct scope *scope, arith *addr, unsigned short packed);
void EnterVarList(struct node *Idlist, struct type *type, int local);
arith EnterParamList(struct node *fpl, struct paramlist **parlist);
arith EnterParTypes(struct node *fpl, struct paramlist **parlist);

#endif /* ENTER_H_ */
