/*
	Definitions of the externs in global.h.
	Could be generated.
*/

/* $Id$ */

#include	"global.h"


/******** EM Machine capacity parameters ********/

size wsize;
size dwsize;
size psize;
long i_minsw;
long i_maxsw;
unsigned long i_maxuw;
long min_off;
long max_off;
ptr max_addr;
size wsizem1;


/******** EM program parameters ********/

ptr ML;
ptr HB;
ptr DB;
long NProc;
long PreIgnMask;


/******** EM machine registers ********/


long PI;
ptr PC;

ptr HP;
ptr SP;
ptr LB;
ptr AB;

long ES;
int ES_def;

int OnTrap;
long IgnMask;
long TrapPI;

char *FRA;
size FRALimit;
size FRASize;
int FRA_def;


/******** The EM Machine Memory ********/

char *text;

char *data;
ptr HL;

char *stack;
char *stackML;
ptr SL;






