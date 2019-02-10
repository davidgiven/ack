/*
	Definitions of the externs in global.h.
	Could be generated.
*/

/* $Id$ */

#include	"global.h"


/******** EM Machine capacity parameters ********/

size wsize = 0;
size dwsize = 0;
size psize = 0;
long i_minsw = 0;
long i_maxsw = 0;
unsigned long i_maxuw = 0;
long min_off = 0;
long max_off = 0;
ptr max_addr = 0;
size wsizem1 = 0;


/******** EM program parameters ********/

ptr ML = 0;
ptr HB = 0;
ptr DB = 0;
long NProc = 0;
long PreIgnMask = 0;


/******** EM machine registers ********/


long PI = 0;
ptr PC = 0;

ptr HP = 0;
ptr SP = 0;
ptr LB = 0;
ptr AB = 0;

long ES = 0;
int ES_def = 0;

int OnTrap = 0;
long IgnMask = 0;
long TrapPI = 0;

char *FRA = NULL;
size FRALimit = 0;
size FRASize = 0;
int FRA_def = 0;


/******** The EM Machine Memory ********/

char *text = NULL;

char *data = NULL;
ptr HL = 0;

char *stack = NULL;
char *stackML = NULL;
ptr SL = 0;






