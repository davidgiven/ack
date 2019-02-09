/* LLgen generated code from source . */
#include "Lpars.h"
#define LLNOFIRSTS
#define LL_LEXI scanner
/* $Id$ */
#ifdef LL_DEBUG
#include <assert.h>
#include <stdio.h>
#define LL_assert(x)	assert(x)
#else
#define LL_assert(x)	/* nothing */
#endif

extern int LLsymb;

#define LL_SAFE(x)	/* Nothing */
#define LL_SSCANDONE(x)	if (LLsymb != x) LLsafeerror(x)
#define LL_SCANDONE(x)	if (LLsymb != x) LLerror(x)
#define LL_NOSCANDONE(x) LLscan(x)
#ifdef LL_FASTER
#define LLscan(x)	if ((LLsymb = LL_LEXI()) != x) LLerror(x)
#endif

extern unsigned int LLscnt[];
extern unsigned int LLtcnt[];
extern int LLcsymb;

#if LL_NON_CORR
extern int LLstartsymb;
#endif

#define LLsdecr(d)	{LL_assert(LLscnt[d] > 0); LLscnt[d]--;}
#define LLtdecr(d)	{LL_assert(LLtcnt[d] > 0); LLtcnt[d]--;}
#define LLsincr(d)	LLscnt[d]++
#define LLtincr(d)	LLtcnt[d]++

extern int LL_LEXI(void);
extern void LLread(void);
extern int LLskip(void);
extern int LLnext(int);
extern void LLerror(int);
extern void LLsafeerror(int);
extern void LLnewlevel(unsigned int *);
extern void LLoldlevel(unsigned int *);
#ifndef LL_FASTER
extern void LLscan(int);
#endif
#ifndef LLNOFIRSTS
extern int LLfirst(int, int);
#endif
#if LL_NON_CORR
extern void LLnc_recover(void);
#endif
#define LL_SSIZE 4
#define LL_NSETS 6
#define LL_NTERMINALS 31
void LL0_spec(void);
void LLparse(void)
 {
	unsigned int s[LL_NTERMINALS+LL_NSETS+2];
	LLnewlevel(s);
	LLread();
LL0_spec();
	LL_SCANDONE(EOFILE);
	LLoldlevel(s);
}
static char LLsets[] = {
'\302','\361','\0','\0',
'\116','\2','\77','\166',
'\0','\2','\3','\0',
'\116','\0','\30','\164',
'\4','\0','\0','\160',
'\52','\0','\0','\0',
0 };
#define LLindex (LL_index+1)
static short LL_index[] = {0,0,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
29,
30,
22,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
24,
23,
-1,
-1,
-1,
28,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
26,
-1,
27,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
25,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
0,
1,
2,
3,
4,
5,
6,
7,
8,
9,
10,
11,
12,
13,
14,
15,
16,
17,
18,
19,
20,
21,
0 };
#define LL_NEWMESS
/* 
 * Some grammar independent code.
 * This file is copied into Lpars.c.
 */

#ifndef lint
static char *rcsid = "$Id$";
#endif

unsigned int	LLtcnt[LL_NTERMINALS];
unsigned int	LLscnt[LL_NSETS];
int		LLcsymb, LLsymb;
static int	LLlevel;

#if LL_NON_CORR
int		LLstartsymb;
static int	fake_eof = 0;
#endif

#if LL_ANSI_C
#define LL_VOIDCST (void)
void	LLmessage(int);
#else
#define LL_VOIDCST
#endif
#ifdef LL_USERHOOK
#if LL_ANSI_C
static int	LLdoskip(int);
static int	LLuserhook(int, int*);
#else
static int	LLdoskip();
static int	LLuserhook();
#endif
#endif

#ifndef LL_FASTER
void LLscan(int t)
{
	/*
	 * Check if the next symbol is equal to the parameter
	 */

#if LL_NON_CORR
	/* See if the error recovery has eaten an eof */
	if (fake_eof) {
		LLsymb = EOFILE;
		fake_eof = 0;
	}
	else {
		LLsymb = LL_LEXI();
	}

	if (LLsymb == t) {
#else
	if ((LLsymb = LL_LEXI()) == t) {
#endif

#if LL_NON_CORR
		/* Check if a previous parser has 'crashed', in that
		 * case continue with non-correcting parser 
		 */
		if (err_seen && !nc_done) {
			LLnc_recover();
			nc_done = 1;
			/* Remember that the error recovery has eaten an eof */
			fake_eof = 1;
			if (t != LLsymb) {
				LLerror(t);
			}
			else
				return;
		}
#endif
		return;
	}
	/*
	 * If we come here, an error has been detected
	 */
	LLerror(t);
}
#endif

void LLread(void) {

#if LL_NON_CORR
	/* Again, check if another parser has crashed,
	 * in that case intercept and go to the
	 * non-correcting parser
	 */

	if (err_seen && !nc_done) {
		LLnc_recover();
		nc_done = 1;
		/* Pretend we read end of file */
		LLsymb = EOFILE;
		LLcsymb = LLindex[EOFILE];
		fake_eof = 0;
		return;
	}

	if (fake_eof) {
		LLsymb = EOFILE;
		LLcsymb = LLindex[EOFILE];
		fake_eof = 0;
		return;
	}
#endif

	for (;;) {
		if ((LLcsymb = LLindex[(LLsymb = LL_LEXI())]) >= 0) return;
		LLmessage(0);
	}
	/* NOTREACHED */
}

void LLerror(int t)
{
	register int i;

	if (t == EOFILE && LLsymb <= 0) return;
#ifdef LL_NEWMESS
	if (t == EOFILE) {
#ifdef LL_USERHOOK
		static int lst[] = { EOFILE, 0 };
		LL_VOIDCST LLuserhook(EOFILE, lst);
#endif /* LL_USERHOOK */
		if (LLsymb != EOFILE && LLsymb > 0) {
			LLmessage(-1);
			while ((LLsymb = LL_LEXI()) > 0 && LLsymb != EOFILE) 
				/* nothing */ ;
		}
		return;
	}
#endif

#if LL_NON_CORR
	if ((!nc_done) && (LLsymb > 0) && (LLsymb != EOFILE)) {
		LLmessage(0);
		LLnc_recover();
		nc_done = 1;
		LLsymb = EOFILE;
	}
#endif
	
	if ((LLcsymb = LLindex[LLsymb]) < 0) {
		LLmessage(0);
		LLread();
	}
	i = LLindex[t];
	LLtcnt[i]++;
#ifdef LL_USERHOOK
	LL_VOIDCST LLdoskip(t);
#else
	LL_VOIDCST LLskip();
#endif
	LLtcnt[i]--;
	if (LLsymb != t) {
#if LL_NON_CORR
		/* A little kludge here; when using non-correcting recovery
		 * it can happen that a program is correct but incomplete. 
		 * Here, we test this, and make sure the appropriate 
		 * message is generated 
	 	 */
		if (! nc_done) {
			int oldLLsymb;
			oldLLsymb = LLsymb;
			LLsymb = EOFILE;
			LLmessage(0);
			nc_done = 1;	
		/* Not really, but to prevent more than 1 error message */
			LLsymb = oldLLsymb;
		}
#endif
		LLmessage(t);
	}
}

void LLsafeerror(int t)
{
	if (t == EOFILE && LLsymb <= 0) return;
#ifdef LL_NEWMESS
	if (t == EOFILE) {
#ifdef LL_USERHOOK
		static int lst[] = { EOFILE, 0 };
		LL_VOIDCST LLuserhook(EOFILE, lst);
#endif /* LL_USERHOOK */
		if (LLsymb != EOFILE && LLsymb > 0) {
			LLmessage(-1);
			while ((LLsymb = LL_LEXI()) > 0 && LLsymb != EOFILE) 
				/* nothing */ ;
		}
		return;
	}
#endif
#if LL_NON_CORR
	if ((!nc_done) && (LLsymb > 0) && (LLsymb != EOFILE)) {
		LLmessage(0);
		LLnc_recover();
		nc_done = 1;
		LLsymb = EOFILE;
	}
	/* A little kludge here; when using non-correcting recovery
         * it can happen that a program is correct but incomplete.
         * Here, we test this, and make sure the appropriate
         * message is generated
         */
        if (! nc_done) {
                int oldLLsymb;
                oldLLsymb = LLsymb;
                LLsymb = EOFILE;
                LLmessage(0);
                nc_done = 1;
        /* Not really, but to prevent more than 1 error message */
                LLsymb = oldLLsymb;
        }
#endif
	LLmessage(t);
}

#ifndef LLNOFIRSTS
#if LL_ANSI_C
int LLfirst(int x, int d) {
#else
int LLfirst(x, d) {
#endif
	register int i;

	return (i = LLindex[x]) >= 0 &&
		(LLsets[d + (i >> 3)] & (1 << (i & 07)));
}
#endif

int LLnext(int n)
{
	/*	returns: 0 if the current symbol is'nt skipped, and it
			 is'nt a member of "n",
			 1 if we have a new symbol, but it is'nt a member,
			 2 if the current symbol is a member,
			 and 3 if we have a new symbol and it is a member.
		So, the low order bit indicates wether we have a new symbol,
		and the next bit indicates wether it is a member of "n".
	*/

	int retval = 0;

	if (LLskip()) retval = 1;
	if (n <= 0 && LLsets[(LLcsymb >> 3) - n] & (1 << (LLcsymb & 07))) {
		retval |= 2;
	}
	else if (n > 0 && LLcsymb == LLindex[n]) retval |= 2;
	return retval;
}

int LLskip(void) {
	/*	returns 0 if the current symbol is'nt skipped, and
		1 if it is, t.i., we have a new symbol
	*/
#ifdef LL_USERHOOK
	return LLdoskip(0);
}

extern void LL_USERHOOK(int, int *);
static int LLuserhook(int e, int *list)
{
	int old = LLsymb;
	LL_USERHOOK(e, list);
	LLread();
	return LLsymb != old;
}

static void LLmklist(register int *list)
{
	char Xset[LL_SSIZE];
	register char *p;
	register int i;

	for (p = &Xset[0]; p < &Xset[LL_SSIZE]; ) *p++ = 0;
	for (i = 0; i < LL_NTERMINALS; i++) {
		if (LLtcnt[i] != 0) Xset[i >> 3] |= (1 << (i & 07));
	}
	for (i = LL_NSETS - 1; i >= 0; i--) if (LLscnt[i] != 0) {
		register char *q = &LLsets[LL_SSIZE * i];

		p = &Xset[0];
		while (p < &Xset[LL_SSIZE]) *p++ |= *q++;
	}
	for (i = 0; i < LL_NTERMINALS; i++) {
		if (Xset[i >> 3] & (1 << (i & 07))) {
			*list++ = LLtok[i];
		}
	}
	*list = 0;
}

static int LLdoskip(int e)
{
	int LLx;
	int list[LL_NTERMINALS+1];
#endif /* LL_USERHOOK */
	register int i;
	int retval;
	int LLi, LLb;

	retval = 0;
#ifdef LL_USERHOOK
	LLmklist(list);
	LLx = LLuserhook(e, list);
	if (LLx) retval = 1;
#endif /* LL_USERHOOK */
	for (;;) {
		if (LLtcnt[LLcsymb] != 0) {
#ifdef LL_USERHOOK
			if (!e || !LLx || LLcsymb == LLindex[e])
#endif
			return retval;
		}
		LLi = LLcsymb >> 3;
		LLb = 1 << (LLcsymb & 07);
		for (i = LL_NSETS - 1; i >= 0; i--) {
			if (LLscnt[i] != 0) {
				if (LLsets[LL_SSIZE*i+LLi] & LLb) {
#ifdef LL_USERHOOK
					if (!e || !LLx || LLcsymb == LLindex[e])
#endif
					return retval;
				}
			}
		}
#ifdef LL_USERHOOK
		if (LLx) {
			LLx = LLuserhook(e, list);
			continue;
		}
#endif /* LL_USERHOOK */
#if LL_NON_CORR
		if ((!nc_done) && (LLsymb > 0)) {
			LLmessage(0);
			LLnc_recover();
			nc_done = 1;
			fake_eof = 1;
		}
		else {
			LLmessage(0);
		}
#else
		LLmessage(0);
#endif
		retval = 1;
		LLread();
	}
	/* NOTREACHED */
}

void LLnewlevel(unsigned int *LLsinfo) {
	register int i;

	if (LLlevel++) {
		LLsinfo[LL_NSETS+LL_NTERMINALS] = (unsigned) LLsymb;
		LLsinfo[LL_NSETS+LL_NTERMINALS+1] = (unsigned) LLcsymb;
		for (i = LL_NTERMINALS - 1; i >= 0; i--) {
			LLsinfo[i] = LLtcnt[i];
			LLtcnt[i] = 0;
		}
		for (i = LL_NSETS - 1; i >= 0; i--) {
			LLsinfo[LL_NTERMINALS+i] = LLscnt[i];
			LLscnt[i] = 0;
		}
	}
	LLtincr(0);
}

void LLoldlevel(unsigned int *LLsinfo) {
	register int i;

	LLtdecr(0);
#ifdef LL_DEBUG
	for (i = 0; i < LL_NTERMINALS; i++) LL_assert(LLtcnt[i] == 0);
	for (i = 0; i < LL_NSETS; i++) LL_assert(LLscnt[i] == 0);
#endif
	if (--LLlevel) {
		for (i = LL_NSETS - 1; i >= 0; i--) {
			LLscnt[i] = LLsinfo[LL_NTERMINALS+i];
		}
		for (i = LL_NTERMINALS - 1; i >= 0; i--) {
			LLtcnt[i] = LLsinfo[i];
		}
		LLsymb = (int) LLsinfo[LL_NSETS+LL_NTERMINALS];
		LLcsymb = (int) LLsinfo[LL_NSETS+LL_NTERMINALS+1];
	}
}
