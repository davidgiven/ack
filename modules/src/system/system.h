/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* RCS: $Id$ */
#ifndef __SYSTEM_INCLUDED__
#define __SYSTEM_INCLUDED__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

extern off_t sys_filesize(char *);
/* Return the temporary directory location */
extern char* sys_gettmpdir(void);
/* Call another program. */
extern int sys_system(const char* prog, const char* const* argv);

/* Extract the base name from a full path specification
 * in "str" and returns it in "dst".
 *
 * "dst" should be large enough to receive the copied
 * data.
 *
 * Supports both DOS and UNIX style paths.
 * */
extern void sys_basename(const char *str, char *dst);

/* Creates a temporary filename, makes it, and returns a malloc'd string
 * containing the filename. */
extern char* sys_maketempfile(const char* prefix, const char* suffix);

#if defined WIN32
/* Really? */
extern char* strndup(const char* s, size_t n);
#endif

/* Sets stdin, stdout or stderr to binary mode --- required on Windows, noop on
 * sane systems. */
extern void sys_setbinarymode(FILE* fp);

/* As system sprintf(), except uses malloc() to allocate a new buffer of the
 * right size for the result. */
extern char* aprintf(const char* format, ...);

#endif /* __SYSTEM_INCLUDED__ */
