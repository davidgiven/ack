/* $Header$ */
/* SYSTEM DEPENDANT DEFINITIONS */

#include <sys/types.h>
#include <errno.h>

#define OP_RDONLY	0	/* open for read */
#define OP_WRONLY	1	/* open for write */
#define OP_CREAT	2	/* create and open for write */
#define OP_APPEND	3	/* open for write at end */

#define sys_open(name, flag)	xopen(name, flag, 0)
#define sys_close(fildes)	xclose(fildes)
#define sys_read(fildes, buffer, nbytes)	read(fildes, buffer, nbytes)
#define sys_write(fildes, buffer, nbytes)	write(fildes, buffer, nbytes)
#define sys_creat(name, mode)	xopen(name, OP_CREAT, mode)
#define sys_remove(name)	unlink(name)
#define sys_fsize(fd)		xfsize(fd)
#define sys_sbrk(incr)		sbrk(incr)
#define sys_stop(how, stat)	xstop(how, stat)

#define S_ABORT	1
#define S_EXIT	2

char *sbrk();
long xfsize();

extern int errno;

#define sys_errno	errno

#define time_type	time_t
#define sys_time(tloc)	time(tloc)
time_type time();
