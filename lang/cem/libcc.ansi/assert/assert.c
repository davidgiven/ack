/*
 * assert.c - diagnostics
 */
/* $Header$ */

#include	<stdio.h>

void _bad_assertion(const char *expr, const char *file, int line) {

	fprintf(stderr,"Assertion \"%s\" failed, file \"%s\", line %d\n",
			expr, file, line);
}
