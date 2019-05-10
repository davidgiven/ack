/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef SET_H_
#define SET_H_

#include <cgg_cg.h>

#define BIS(sp,n) (sp)[(n)>>4] |= 1<<((n)&0xF)
#define BIC(sp,n) (sp)[(n)>>4] &= ~(1<<((n)&0xF))
#define BIT(sp,n) (((sp)[(n)>>4]&(1<<((n)&0xF)))!=0)

int setlookup(set_t s);
void make_std_sets(void);
set_t ident_to_set(char *name);
set_t setproduct(set_t s1, set_t s2);
set_t setsum(set_t s1, set_t s2);
set_t setdiff(set_t s1, set_t s2);


#endif /* SET_H_ */
