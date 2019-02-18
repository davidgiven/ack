/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-06
 *  
 */
#ifndef DECLARATOR_H_
#define DECLARATOR_H_

#include "arith.h"

/* Forward declarations. */
struct type;
struct declarator;
struct formal;
struct proto;
struct expr;

struct type *
declare_type(struct type *tp, struct declarator *dc);
void add_decl_unary(register struct declarator *dc, int fund, int qual,
		arith count, struct formal *fm, struct proto *pl);
void remove_declarator(struct declarator *dc);
void reject_params(register struct declarator *dc);
void check_array_subscript(register struct expr *expr);

#endif /* DECLARATOR_H_ */
