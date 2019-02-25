/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef CASESTAT_H_
#define CASESTAT_H_

#include <em_label.h>

struct node;

void CaseExpr(struct node *nd);
void CaseEnd(struct node *nd, label exit_label);




#endif /* CASESTAT_H_ */
