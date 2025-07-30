/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	DATAFLOW ANALYSIS ON C PROGRAMS	*/

/*	Compile the C compiler with flag DATAFLOW.
    Use the compiler option --d.
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "parameters.h" /* UF */
#include "dataflow.h"

#ifdef DATAFLOW
char* CurrentFunction = 0;
int NumberOfCalls;

void DfaStartFunction(char* nm)
{
	CurrentFunction = nm;
	NumberOfCalls = 0;
}

void DfaEndFunction(void)
{
	if (NumberOfCalls == 0)
		printf("DFA: %s: --none--\n", CurrentFunction);
}

void DfaCallFunction(char* s)
{
	printf("DFA: %s: %s\n", CurrentFunction, s);
	++NumberOfCalls;
}
#endif /* DATAFLOW */
