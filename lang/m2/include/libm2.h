#ifndef LIBM2_H
#define LIBM2_H

#include <stdint.h>

struct array_descr
{
	int lbound;
	int n_elts_min_one;
	unsigned size;
};

struct int_range_descr
{
	int low, high;
};

struct uint_range_descr
{
	unsigned int low, high;
};

struct long_range_descr
{
	long low, high;
};

struct ulong_range_descr
{
	unsigned long low, high;
};

struct stack_descr
{
	char* addr;
	int low;
	unsigned int highminlow;
	unsigned int size;
};

struct proc
{
	unsigned size; /* size of saved stackframe(s) */
	int (*proc)(void); /* address of coroutine procedure */
	char* brk; /* stack break of this coroutine */
};

extern void (*handler)(int);
extern char** argv;
extern int argc;
extern char* MainLB;

extern double absd(double i);
extern int CallAtEnd(void (*p)(void));
extern int absi(int i);
extern int dvi(int j, int i);
extern int rmi(int j, int i);
extern int sigtrp(int trapno, int signo);
extern int stackprio(unsigned n);
extern int topsave(void* brkpos, struct proc* proc);
extern long absl(long i);
extern long dvil(long j, long i);
extern long rmil(long j, long i);
extern size_t new_stackptr(struct stack_descr* pdscr, int a);
extern unsigned int topsize(void* brkpos);
extern void SIG(void (*)(int));
extern void StringAssign(int dstsiz, int srcsiz, register char* dstaddr, register char* srcaddr);
extern void TRP(int trapno);
extern void _Arguments_(void);
extern void _SYSTEM__NEWPROCESS(int (*p)(void), struct proc* a, unsigned n, struct proc** p1);
extern void _SYSTEM__TRANSFER(struct proc** a, struct proc** b);
extern void _cleanup(void);
extern void adduchk(unsigned a, unsigned b);
extern void blockmove(size_t siz, char* dst, char* src);
extern void cap(unsigned u);
extern void catch (int trapno);
extern void copy_array(char* pp, int a);
extern void halt(void);
extern void init(void);
extern void killbss(void);
extern void load(size_t siz, register char* addr, int p);
extern void muluchk(unsigned a, unsigned b);
extern void rcka(struct array_descr* descr, int indx);
extern void rcki(struct int_range_descr* descr, int val);
extern void rckil(struct long_range_descr* descr, long val);
extern void rcku(struct uint_range_descr* descr, unsigned val);
extern void rckul(struct ulong_range_descr* descr, unsigned long val);
extern void store(size_t siz, register char* addr, int p);
extern void subuchk(unsigned a, unsigned b);
extern void topload(struct proc* proc);
extern void unstackprio(unsigned n);

/* PROCEDURE Argv(argnum: CARDINAL; VAR argument: ARRAY OF CHAR): CARDINAL; */
extern unsigned _Arguments__Argv(int n, char* argument, int l, unsigned int u, int s);

/* PROCEDURE GetEnv(name: ARRAY OF CHAR; VAR value: ARRAY OF CHAR): CARDINAL; */
extern unsigned _Arguments__GetEnv(
    char* name, int nn, unsigned int nu, int ns, char* value, int l, unsigned int u, int s);

/* PROCEDURE Message(str: ARRAY OF CHAR); */
extern void _Traps__Message(char* str, int nn, unsigned int nu, int ns);

#endif
