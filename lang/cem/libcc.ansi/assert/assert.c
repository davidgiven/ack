/*
 * assert.c - diagnostics
 */
/* $Header$ */

#include	<assert.h>
#include	<stdio.h>
#include	<stdlib.h>

void __bad_assertion(const char *expr, const char *file, int line) {

	fprintf(stderr,"Assertion \"%s\" failed, file \"%s\", line %d\n",
			expr, file, line);
	abort();
}
