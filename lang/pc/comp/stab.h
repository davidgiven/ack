/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-22
 *  
 */
#ifndef STAB_H_
#define STAB_H_

struct def;
struct type;

void stb_string(register struct def *df, long kind);
void stb_addtp(char *s, struct type *tp);

#endif /* STAB_H_ */
