/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* TOKENNAME DEFINITION */
#ifndef TOKENNAME_H_
#define TOKENNAME_H_

struct tokenname	{	/*	Used for defining the name of a
					token as identified by its symbol
				*/
	int tn_symbol;
	char *tn_name;
};

void reserve(register struct tokenname resv[]);

#endif /* TOKENNAME_H_ */
