/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef __PRINT_INCLUDED__
#define __PRINT_INCLUDED__

#include <ansi.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

_PROTOTYPE(void print, (char *fmt, ...));
_PROTOTYPE(void fprint, (File *f, char *fmt, ...));
_PROTOTYPE(void doprnt, (File *f, char *fmt, va_list ap));
_PROTOTYPE(int _format, (char *buf, char *fmt, va_list ap));
_PROTOTYPE(char *sprint, (char *buf, char *fmt, ...));

#endif /* __PRINT_INCLUDED__ */
