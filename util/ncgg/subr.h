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

extern void n_proc(char* name);
extern struct varinfo* make_erase(char* name);
extern void
n_instr(char* name, char* asname, operand* oplist, struct varinfo* eraselist, struct varinfo* cost);
extern void n_set(char* name, int number);
extern void
n_tok(char* name, struct varinfo* atts, int size, struct varinfo* cost, struct varinfo* format);
extern void checkprintformat(int n);
extern void n_prop(char* name, int size);
extern void prophall(int n);
extern int n_reg(char* name, char* printstring, int nmemb, int member1, int member2);
extern void make_const(void);
extern int cmustbeset(char* ident);
extern void n_const(char* ident, int val);
extern void n_sconst(char* ident, char* val);
extern void regline(struct varinfo* rl, struct varinfo* pl, int rv);
extern void check_reglap(void);
extern void setallreg(struct varinfo* vi);
extern void freevi(struct varinfo* vip);
extern int myatoi(char* s);
extern char* mystrcpy(char* s);
extern char* myalloc(int n);
extern int chkincl(int value, int lwb, int upb);
extern int subset(short* sp1, short* sp2, int setsize);
extern int vilength(struct varinfo* vip);
extern char* mygetcwd(void);

#endif /* SUBR_H_ */
