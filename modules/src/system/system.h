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

struct _sys_fildes {
	FILE* fd;
};

typedef struct _sys_fildes File;

extern File _sys_ftab[];
extern File sys_stdin;
extern File sys_stdout;
extern File sys_stderr;

/* flags for sys_open() */
#define OP_READ		01
#define OP_WRITE	02
#define OP_APPEND	04

/* flags for sys_access() */
#define AC_EXIST	00
#define AC_READ		04
#define AC_WRITE	02
#define AC_EXEC		01

/* flags for sys_stop() */
#define S_END	0
#define S_EXIT	1
#define S_ABORT	2

extern FILE* getfd(File* fp);

int sys_open(char *, int, File **);
void sys_close(File *);
int sys_read(File *, char *, int, int *);
int sys_write(File *, char *, int);
int sys_seek(File *, long, int, long *);
int sys_reset(File *);
int sys_access(char *, int);
int sys_remove(char *);
int sys_rename(char *, char *);
off_t sys_filesize(char *);
int sys_chmode(char *, int);
/* Return the temporary directory location */
char* sys_gettmpdir(void);
/* Call another program. */
int sys_system(const char* prog, const char* const* argv);
#if 0
int sys_lock(char *);
int sys_unlock(char *);
#endif
NORETURN void sys_stop(int);
time_t sys_modtime(char *);

/* standard file decsriptors */
#define STDIN	&sys_stdin
#define STDOUT	&sys_stdout
#define STDERR	&sys_stderr

/* maximum number of open files */
#define SYS_NOPEN	20

/* return value for sys_break */
#define ILL_BREAK	((char *)0)


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

/* Implementation definitions. */

extern File *_get_entry(void);

#endif /* __SYSTEM_INCLUDED__ */
