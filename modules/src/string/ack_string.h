/*
 * (c) copyright 1993 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* RCS: $Id: */

#ifndef __ACK_STRING_INCLUDED__
#define __ACK_STRING_INCLUDED__

char *strindex(char *s, int c);
char *strrindex(char *s, int c);
char *strzero(char *s);
char *str2bts(register char *str, char *bts, int *pn);
char *long2str(long val, int base);
long str2long(char* str, int base);
char *btscpy(char *s1, char *s2, int n);
char *btscat(char *s1, int n1, char *s2, int n2);
int btscmp(char *s1, int n1, char *s2, int n2);
char *btszero(char *b, int n);
char *bts2str(char *b, int n, char *s);

#endif /* __ACK_STRING_INCLUDED__ */
