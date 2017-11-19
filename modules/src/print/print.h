/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef __PRINT_INCLUDED__
#define __PRINT_INCLUDED__

#include <stdarg.h>

void print(char *fmt, ...);
void fprint(File *f, char *fmt, ...);
void doprnt(File *f, char *fmt, va_list ap);
int _format(char *buf, char *fmt, va_list ap);
char *sprint(char *buf, char *fmt, ...);

#endif /* __PRINT_INCLUDED__ */
