/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/* File containing definitions of the UNIX system calls and some other
 * functions.
 * Used to fine-tune the -lc library.
 */

	/* LINTLIBRARY */

#include <stdio.h>
#include <sgtty.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <time.h>

int	access(s, m) char *s; { return(0); }
int	acct(s) char *s; { return(0); }
int	alarm(s) unsigned s; { return(0); }
char	*brk(a) char *a; { return(a); }
char	*sbrk(i) { return((char *)0); }
int	chdir(s) char *s; { return(0); }
int	chroot(s) char *s; { return(0); }
int	chmod(s, m) char *s; { return(0); }
int	chown(s, u, g) char *s; { return(0); }
int	close(f) { return(0); }
int	creat(s, m) char *s; { return(0); }
int	dup(f) { return(0); }
int	dup2(f1, f2) { return(0); }
	/*VARARGS2*/
	execl(f, a) char *f, *a; { ; }
	execv(s, a) char *s, *a[]; { ; }
	/*VARARGS3*/
	execle(f, a, s) char *f, *a, *s; { ; }
	execve(s, a, e) char *s, *a[], *e[]; { ; }
	_exit(s) { ; }
int	fork() { return(0); }
int	getpid() { return(0); }
int	getuid() { return(0); }
int	getgid() { return(0); }
int	geteuid() { return(0); }
int	getegid() { return(0); }
	/*VARARGS2*/
int	ioctl(d, r) int d; unsigned int r; { return(0); }
int	kill(p, s) { return(0); }
int	link(a, b) char *a, *b; { return(0); }
long	lseek(f, o, d) long o; { return(0); }
int	mknod(n, m, a) char *n; { return(0); }
int	mount(s, n, f) char *s, *n; { return(0); }
int	umount(s) char *s; { return(0); }
int	nice(p) { return(0); }
int	open(f, m) char *f; { return(0); }
	pause() { ; }
int	pipe(f) int f[2]; { return(0); }
	profil(b, s, o, i) char *b; { ; }
int	ptrace(r, p, a, d) int *a;{ return(0); }
int	read(f, b, l) char *b; { return(0); }
int	setuid(u) { return(0); }
int	setgid(g) { return(0); }
#ifndef __STDC__
int	(*signal(c, f))() int (*f)(); { return(f); }
#endif
int	stat(s, b) char *s; struct stat *b; { return(0); }
int	fstat(f, b) struct stat *b; { return(0); }
int	stime(t) time_t *t; { return(0); }
	sync() { ; }
time_t	time(t) time_t *t; { return( 0 ); }
	ftime(t) struct timeb *t; { ; }
	times(b) struct tbuf *b; { ; }
int	umask(c) { return(0); }
int	unlink(s) char *s; { return(0); }
int	utime(f, t) char *f; time_t t[2]; { return(0); }
int	wait(s) int *s; { return(0); }
int	write(f, b, l) char *b; { return(0); }
int	abort() {/*NOTREACHED*/ }

/* Some definitions of format functions */
/* FORMAT0 $
	%[dox] = int		%l[dox] = long		%[DOX] = long
	%u = unsigned int	%lu = unsigned long	%U = unsigned long
	%[feg] = double
	%c = int		%s = char *
$ */
#ifdef __STDC__
int     printf(const char *_format, ...) { return(0); }
#else
printf(fmt) char *fmt; { ; }
#endif

/* FORMAT1 */
#ifdef __STDC__
int     fprintf(FILE *_stream, const char *_format, ...) { return(0); }
#else
fprintf(fp, fmt) FILE *fp; char *fmt; { ; }
#endif
/* FORMAT1 */
#ifdef __STDC__
int     sprintf(char *_s, const char *_format, ...) { return(0); }
#else
sprintf(s, fmt) char *s; char *fmt; { ; }
#endif

/* FORMAT0 $
	%[dox] = int *		%l[dox] = long *	%[DOX] = long *
	%h[dox] = short *
	%u = unsigned int *	%lu = unsigned long *	%U = unsigned long *
	%[fe] = float *		%l[fe] = double *	%[FE] = double *
	%c = char *		%s = char *		%[[] = char *
$ */
#ifdef __STDC__
int     scanf(const char *_format, ...) { return(0); }
#else
int scanf(fmt) char *fmt; { return(0); }
#endif
/* FORMAT1 */
#ifdef __STDC__
int     fscanf(FILE *_stream, const char *_format, ...) { return(0); }
#else
int fscanf(fp, fmt) FILE *fp; char *fmt; { return(0); }
#endif
/* FORMAT1 */
#ifdef __STDC__
int     sscanf(const char *_s, const char *_format, ...) { return(0); }
#else
int sscanf(s, fmt) char *s; char *fmt; { return(0); }
#endif
int _flsbuf(i, fp) unsigned int i; FILE *fp; { return(0); }

/* System variables */
char **environ;
int errno;
char *sys_errlist[1];
