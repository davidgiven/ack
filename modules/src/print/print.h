/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef __PRINT_INCLUDED__
#define __PRINT_INCLUDED__

#include <stdarg.h>
#include <system.h>

void fprint(FILE *f, const char *fmt, ...);
void doprnt(FILE *f, const char *fmt, va_list ap);
int _format(char *buf, const char *fmt, va_list ap);

#endif /* __PRINT_INCLUDED__ */
