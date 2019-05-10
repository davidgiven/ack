/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  
 */
#ifndef UTIL_H_
#define UTIL_H_

void error(char *s, ...);
#ifdef DIAGOPT
void optim(int n);
#endif

#endif /* UTIL_H_ */
