/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-04-12
 *  
 */
#ifndef SUBR_H_
#define SUBR_H_

#include "data.h"
#include "types.h"

int match(register token_p tp, register set_p tep, int optexp);
void instance(int instno,register token_p token);
void cinstance(int instno,register token_p token,
		 register token_p tp,int regno);
int eqtoken(token_p tp1,token_p tp2);
int distance(int cindex);
unsigned costcalc(cost_t cost);
int ssize(int tokexpno);
int tsize(register token_p tp);

#ifdef MAXSPLIT
int instsize(int tinstno,token_p tp);
#endif /* MAXSPLIT */

void tref(register token_p tp,int amount);

#ifdef MAXSPLIT
int split(token_p tp,int *ip,int ply,int toplevel);
#endif /* MAXSPLIT */

unsigned docoerc(token_p tp,c3_p cp,int ply,int toplevel,int forced);
unsigned stackupto(token_p limit,int ply,int toplevel);
c3_p findcoerc(token_p tp,set_p tep);
void error(char *s, ...);
void fatal(char *s, ...);

#ifndef NDEBUG
void badassertion(char* asstr,char *file,int line);
#endif

int max(int a,int b);


#endif /* SUBR_H_ */
