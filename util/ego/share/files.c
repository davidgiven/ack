/*  S H A R E D   F I L E
 *
 *  F I L E S . C 
 */

#include <stdio.h>

FILE *openfile(name,mode)
	char *name,*mode;
{
	FILE *f;

	if ((f = fopen(name,mode)) == NULL) {
		error("cannot open %s",name);
	}
	return f;
}
