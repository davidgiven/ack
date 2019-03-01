/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* OPERATOR HANDLING */

#include    "expr.h"
#include	"Lpars.h"

int rank_of(int oper)
{
	/*	The rank of the operator oper is returned.
	*/
	switch (oper)	{
	default:
		return 0;
	case '(':
		return 1;
	case '!':
		return 2;
	case '*':
	case '/':
	case '%':
		return 3;
	case '+':
	case '-':
		return 4;
	case LEFT:
	case RIGHT:
		return 5;
	case '<':
	case '>':
	case LESSEQ:
	case GREATEREQ:
		return 6;
	case EQUAL:
	case NOTEQUAL:
		return 7;
	case '&':
		return 8;
	case '^':
		return 9;
	case '|':
		return 10;
	case AND:
		return 11;
	case OR:
		return 12;
	case '?':
	case ':':
		return 13;
	case ',':
		return 15;
	}
	/*NOTREACHED*/
}
