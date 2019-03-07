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

typedef struct type t_type;
typedef struct def  t_def;

void stb_addtp(char *s, t_type *tp);
void stb_string(register t_def *df, int kind);


#endif /* STAB_H_ */
