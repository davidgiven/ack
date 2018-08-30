/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef __PRINT_INCLUDED__
#define __PRINT_INCLUDED__

#include <stdarg.h>

void print(const char *fmt, ...);
void fprint(File *f, const char *fmt, ...);
void doprnt(File *f, const char *fmt, va_list ap);
int _format(char *buf, const char *fmt, va_list ap);
char *sprint(char *buf, const char *fmt, ...);

#endif /* __PRINT_INCLUDED__ */
