/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-04-12
 *  
 */
#ifndef GENCODE_H_
#define GENCODE_H_

#include "data.h"

void out_init(char *filename);
void out_finish(void);
void tstoutput(void);
void gencode(register char *code);
void genexpr(int nodeno);
void gennl(void);
void prtoken(token_p tp);


#endif /* GENCODE_H_ */
