/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
#define INP_PUSHBACK 3

#include <inp_pkg.spec>

/*	Note: The following macro only garuantees one PushBack.
*/
#define UnGetChar()	ChPushBack(LexSave)

extern	int LexSave;	/* last character read by GetChar		*/
extern 	int GetChar();	/* character input, with trigraph parsing	*/
