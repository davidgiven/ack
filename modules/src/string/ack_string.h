/*
 * (c) copyright 1993 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* RCS: $Header: */

#ifndef __ACK_STRING_INCLUDED__
#define __ACK_STRING_INCLUDED__

#include <ansi.h>

_PROTOTYPE(char *strcpy, (char *s1, const char *s2));
_PROTOTYPE(char *strncpy, (char *s1, const char *s2, size_t n));
_PROTOTYPE(char *strcat, (char *s1, const char *s2));
_PROTOTYPE(char *strncat, (char *s1, const char *s2, size_t n));
_PROTOTYPE(int strcmp, (const char *s1, const char *s2));
_PROTOTYPE(int strncmp, (const char *s1, const char *s2, size_t n));
_PROTOTYPE(_SIZET strlen, (const char *s));
_PROTOTYPE(char *strindex, (char *s, int c));
_PROTOTYPE(char *strrindex, (char *s, int c));
_PROTOTYPE(char *strzero, (char *s));
_PROTOTYPE(char *str2bts, (char *s, char *b, int *n));
_PROTOTYPE(char *long2str, (long l, int b));
_PROTOTYPE(long str2long, (char *s, int b));
_PROTOTYPE(char *btscpy, (char *s1, char *s2, int n));
_PROTOTYPE(char *btscat, (char *s1, int n1, char *s2, int n2));
_PROTOTYPE(int btscmp, (char *s1, int n1, char *s2, int n2));
_PROTOTYPE(char *btszero, (char *b, int n));
_PROTOTYPE(char *bts2str, (char *b, int n, char *s));

#endif /* __ACK_STRING_INCLUDED__ */
