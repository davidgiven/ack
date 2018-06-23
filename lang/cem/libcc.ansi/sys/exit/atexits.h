#ifndef ATEXITS_H
#define ATEXITS_H

#define NEXITS 32

extern void (*__functab[NEXITS])(void);
extern int __funccnt;

#endif
