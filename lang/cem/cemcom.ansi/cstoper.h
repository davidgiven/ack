/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-06
 *
 */
#ifndef CSTOPER_H_
#define CSTOPER_H_

struct expr;

void cstbin(struct expr** expp, int oper, struct expr* expr);
void cut_size(struct expr* expr);
void init_cst(void);

#endif /* CSTOPER_H_ */
