/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-27
 *  
 */
#ifndef STAB_H_
#define STAB_H_
/* D E B U G G E R   S Y M B O L   T A B L E */


struct def;

void stb_addtp(char *s, struct type *tp);
void stb_string(register struct def *df, int kind);


#endif /* STAB_H_ */
