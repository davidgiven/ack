/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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
