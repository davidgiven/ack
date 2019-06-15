/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-05-02
 *  
 */
#ifndef SUBR_H_
#define SUBR_H_

#include "instruct.h"

struct varinfo;


void n_proc(char *name);
struct varinfo * make_erase(char *name);
void n_instr(char *name, char *asname, operand *oplist, struct varinfo *eraselist,
		  struct varinfo *cost);
void n_set(char *name, int number);
void n_tok(char *name, struct varinfo *atts, int size,
		 struct varinfo *cost, struct varinfo *format);
void checkprintformat(int n);
void n_prop(char *name, int size);
void prophall(int n);
int n_reg(char *name, char *printstring, int nmemb, int member1, int member2);
void make_const(void);
int cmustbeset(char *ident);
void n_const(char *ident, int val);
void n_sconst(char *ident, char *val);
void regline(struct varinfo *rl, struct varinfo *pl, int rv);
void check_reglap(void);
void setallreg(struct varinfo *vi);
void freevi(register struct varinfo *vip);
int myatoi(register char *s);
char *mystrcpy(char *s);
char *myalloc(register int n);
int chkincl(int value, int lwb, int upb);
int subset(short *sp1, short *sp2, int setsize);
int vilength(register struct varinfo *vip);

#endif /* SUBR_H_ */
