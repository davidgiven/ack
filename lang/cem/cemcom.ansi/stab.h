/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-07
 *  
 */
#ifndef STAB_H_
#define STAB_H_


struct def;
struct type;
struct tag;

void stb_tag(register struct tag *tg, char *str);
void stb_typedef(register struct type *tp, char *str);
void stb_string(register struct def *df, int kind, char* str);

#endif /* STAB_H_ */
