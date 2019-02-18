/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-07
 *  
 */
#ifndef FLTCSTOPER_H_
#define FLTCSTOPER_H_

struct expr;

void fltcstbin(register struct expr **expp, int oper, register struct expr *expr);

#endif /* FLTCSTOPER_H_ */
