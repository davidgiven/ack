/*
	The MON instruction
*/

/* $Header$ */

#include	"sysidf.h"
#include	"log.h"
#include	"alloc.h"
#include	"shadow.h"

#include	<sys/types.h>
#include	<sys/stat.h>
#if __STDC__
#include	<time.h>
#endif
#include	<sys/times.h>

extern int errno;			/* UNIX error number */

extern int running;			/* from main.c */
extern int fd_limit;			/* from io.c */

#define	good_fd(fd)	(fd < fd_limit ? 1 : (errno = 9 /* EBADF */, 0))

#ifdef	BSD_X				/* from system.h */
#include	<sys/timeb.h>
#endif	/* BSD_X */
#ifdef	SYS_V
struct timeb {			/* non-existing; we use an ad-hoc definition */
	long time;
	unsigned short millitm;
	short timezone, dstflag;
};
#endif	/* SYS_V */

#ifdef	BSD4_2				/* from system.h */
#include	<sys/time.h>
#endif	/* BSD4_2 */

#ifdef	SYS_V
#include	<sys/errno.h>
#undef		ERANGE			/* collision with trap.h */
#include	<fcntl.h>
#include	<time.h>
#endif	/* SYS_V */

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"trap.h"
#include	"warn.h"
#include	"mem.h"

#define	INPUT		0
#define	OUTPUT		1

#define	DUPMASK		0x40

extern long lseek();
#ifdef	SYS_V
extern unsigned int alarm();
extern long time();
extern void sync();
#endif	/* SYS_V */

#define	INT2SIZE	max(wsize, 2L)
#define	INT4SIZE	max(wsize, 4L)

#define	pop_int()	((int) swpop())
#define	pop_int2()	((int) spop(INT2SIZE))
#define	pop_int4()	(spop(INT4SIZE))
#define	pop_intp()	((int) spop(psize))
#define	pop_uns2()	((unsigned int) upop(INT2SIZE))
#define	pop_unsp()	((unsigned int) upop(psize))
#define	pop_ptr()	(dppop())

#define	push_int(a)	(wpush((long)(a)))
#define	push_i2(a)	(npush((long)(a), INT2SIZE))
#define	push_i4(a)	(npush((long)(a), INT4SIZE))
#define	push_unsp(a)	(npush((long)(a), psize))

#define	push_err()	{ push_int(errno); push_int(errno); }

/************************************************************************
 *				Monitor calls.				*
 *									*
 *	The instruction "MON" expects a wsized integer on top of	*
 *	top of the stack, which identifies the call. Often there	*
 *	are also parameters following this number. The parameters	*
 *	were stacked in reverse order (C convention).			*
 *	The possible parameter types are :				*
 *									*
 *		1) int : integer of wordsize				*
 *		2) int2: integer with size max(2, wordsize)		*
 *		3) int4: integer with size max(4, wordsize)		*
 *		4) intp: integer with size of a pointer			*
 *		5) uns2: unsigned integer with size max(2, wordsize)	*
 *		6) unsp: unsigned integer with size of a pointer	*
 *		7) ptr : pointer into data space			*
 *									*
 *	After the call has been executed, a return code is present	*
 *	on top of the stack. If this return code equals zero, the call	*
 *	succeeded and the results of the call can be found right	*
 *	after the return code. A non zero return code indicates a	*
 *	failure.  In this case no results are available and the return	*
 *	code has been pushed twice.					*
 *									*
 *	Monitor calls such as "ioctl", "stat", "ftime", etc. work	*
 *	with a private buffer to be filled by the call. The fields	*
 *	of the buffer are written to EM-memory separately, possibly	*
 *	with some of the fields aligned.  To this end a number of	*
 *	transport routines are assembled in monstruct.[ch].		*
 *									*
 *	The EM report specifies a list of UNIX Version 7 -like system	*
 *	calls, not full access to the system calls on the underlying	*
 *	machine.  Therefore an attempt has been made to use or emulate	*
 *	the Version 7 system calls on the various machines.  A number	*
 *	of 4.1 BSD specific parameters have also been implemented.	*
 *									*
 ************************************************************************/

PRIVATE size buf_cnt[5];		/* Current sizes of the buffers */
PRIVATE char *buf[5];			/* Pointers to the buffers */

PRIVATE check_buf();
PRIVATE int savestr();
PRIVATE int vec();

moncall()
{
	int n;				/* number actually read/written */
	int status;			/* status for wait-call */
	int flag;			/* various flag parameters */
	int mode;			/* various mode parameters */
	int oldmask;			/* for umask call */
	int whence;			/* parameter for lseek */
	int address;			/* address parameter typed int2 */
	int owner;			/* owner parameter typed int2 */
	int group;			/* group parameter typed int2 */
	int pid;			/* pid parameter typed int2 */
	int ppid;			/* parent process pid */
	long off;			/* offset parameter */
	int pfds[2];			/* pipe file descriptors */
	long tm;			/* for stime call */
	long actime, modtime;		/* for utime */
	int incr;			/* for nice call */
	int fd, fdnew;			/* file descriptors */
	int groupid;			/* group id */
	int userid;			/* user id */
	int sig;			/* killing signal */
	ptr dsp1, dsp2, dsp3;		/* Data Space Pointers */
	int nbytes;			/* number to be read/written */
	unsigned int seconds;		/* for alarm call */
	int trap_no;			/* for sigtrp; trap number */
	int old_trap_no;		/* for sigtrp; old trap number */
	int sig_no;			/* for sigtrp; signal number */
	int request;			/* ioctl and ptrace request */
	char **envvec;			/* environment vector (exec) */
	char **argvec;			/* argument vector (exec) */

	struct stat st_buf;		/* private stat buffer */
	struct tms tm_buf;		/* private tms buffer */
	struct timeb tb_buf;		/* private timeb buffer */

#ifdef	BSD4_2				/* from system.h */
	struct timeval tv;		/* private timeval buffer */
#endif	/* BSD4_2			 */

#ifdef	SYS_V				/* from system.h */
	struct {time_t x, y;} utimbuf;	/* private utime buffer */
#else	/* SYS_V */
	time_t utimbuf[2];		/* private utime buffer */
#endif	/* !SYS_V */

	char *cp;
	int nr;
	ptr addr;
	int rc;

	switch (pop_int()) {

	case 1:				/* Exit */

#ifdef	LOGGING
		ES_def =
			((st_sh(SP) == UNDEFINED)
				|| (st_sh(SP + wsize-1) == UNDEFINED)) ?
			UNDEFINED : DEFINED;
#else
		ES_def = DEFINED;
#endif	/* LOGGING */
		ES = pop_int();
		running = 0;		/* stop the machine */
		LOG(("@m9 Exit: ES = %ld", ES));
		break;

	case 2:				/* Fork */

		ppid = getpid();
		if ((pid = fork()) == 0) {
			/* Child */
			init_ofiles(0);	/* Reinitialize */
			push_int(ppid);	/* Pid of parent */
			push_int(1);	/* Flag = 1 for child */
			push_int(0);
			LOG(("@m9 Fork: in child, ppid = %d", ppid));
		}
		else if (pid > 0) {	/* Parent */
			incr_mess_id();	/* Incr. id for next child */
			push_int(pid);	/* Pid of child */
			push_int(0);	/* Flag = 0 for parent */
			push_int(0);
			LOG(("@m9 Fork: in parent, cpid = %d", pid));
		}
		else {
			/* fork call failed */
			push_err();
			LOG(("@m4 Fork: failed, pid = %d, errno = %d",
				pid, errno));
		}
		break;

	case 3:				/* Read */

		fd = pop_int();
		dsp1 = pop_ptr();
		nbytes = pop_intp();

		if (!good_fd(fd))
			goto read_error;
		if (nbytes < 0) {
			errno = 22;	/* EINVAL */
			goto read_error;
		}

		check_buf(0, (size)nbytes);
		if ((n = read(fd, buf[0], nbytes)) == -1)
			goto read_error;

#ifdef	LOGGING
		if (check_log("@m6")) {
			register int i;
			
			for (i = 0; i < n; i++) {
				LOG(("@m6 Read: char = '%c'", *(buf[0] + i)));
			}
		}
#endif	/* LOGGING */

		if (in_gda(dsp1) && !in_gda(dsp1 + (n-1))) {
			efault(WRGDAH);
			goto read_error;
		}

		if (!is_in_mem(dsp1, n)) {
			efault(WRUMEM);
			goto read_error;
		}

		for (	nr = n, cp = buf[0], addr = dsp1;
			nr;
			nr--, addr++, cp++
		) {
			if (in_stack(addr)) {
				ch_st_prot(addr);
				stack_loc(addr) = *cp;
				st_int(addr);
			}
			else {
				ch_dt_prot(addr);
				data_loc(addr) = *cp;
				dt_int(addr);
			}
		}

		push_unsp(n);
		push_int(0);
		LOG(("@m9 Read: succeeded, n = %d", n));
		break;

	read_error:
		push_err();
		LOG(("@m4 Read: failed, n = %d, errno = %d", n, errno));
		break;

	case 4:				/* Write */

		fd = pop_int();
		dsp1 = pop_ptr();
		nbytes = pop_intp();

		if (!good_fd(fd))
			goto write_error;
		if (nbytes < 0) {
			errno = 22;	/* EINVAL */
			goto read_error;
		}

		if (in_gda(dsp1) && !in_gda(dsp1 + (nbytes-1))) {
			efault(WWGDAH);
			goto write_error;
		}
		if (!is_in_mem(dsp1, nbytes)) {
			efault(WWUMEM);
			goto write_error;
		}

#ifdef	LOGGING
		for (addr = dsp1; addr < dsp1 + nbytes; addr++) {
			if (mem_sh(addr) == UNDEFINED) {
				warning(in_stack(addr) ? WWLUNDEF : WWGUNDEF);
			}
		}
#endif	/* LOGGING */

		check_buf(0, (size)nbytes);
		for (	nr = nbytes, addr = dsp1, cp = buf[0];
			nr;
			nr--, addr++, cp++
		) {
			*cp = mem_loc(addr);
		}

#ifdef	LOGGING
		if (check_log("@m6")) {
			register int i;
			
			for (i = 0; i < nbytes; i++) {
				LOG(("@m6 write: char = '%c'", *(buf[0] + i)));
			}
		}
#endif	/* LOGGING */

		if ((n = write(fd, buf[0], nbytes)) == -1)
			goto write_error;

		push_unsp(n);
		push_int(0);
		LOG(("@m9 Write: succeeded, n = %d", n));
		break;

	write_error:
		push_err();
		LOG(("@m4 Write: failed, n = %d, nbytes = %d, errno = %d",
			n, nbytes, errno));
		break;

	case 5:				/* Open */

		dsp1 = pop_ptr();
		flag = pop_int();
		if (!savestr(0, dsp1) || (fd = open(buf[0], flag)) == -1) {
			push_err();
			LOG(("@m4 Open: failed, file = %lu, flag = %d, fd = %d, errno = %d",
					dsp1, flag, fd, errno));
		}
		else {
			push_int(fd);
			push_int(0);
			LOG(("@m9 Open: succeeded, file = %lu, flag = %d, fd = %d",
					dsp1, flag, fd));
		}
		break;

	case 6:				/* Close */

		fd = pop_int();
		if (!good_fd(fd) || close(fd) == -1) {
			push_err();
			LOG(("@m4 Close: failed, fd = %d, errno = %d",
				fd, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Close: succeeded"));
		}
		break;

	case 7:				/* Wait */

		if ((pid = wait(&status)) == -1) {
			push_err();
			LOG(("@m4 Wait: failed, status = %d, errno = %d",
				status, errno));
		}
		else {
			push_i2(pid);
			push_i2(status);
			push_int(0);
			LOG(("@m9 Wait: succeeded, status = %d, pid = %d",
					status, pid));
		}
		break;

	case 8:				/* Creat */

		dsp1 = pop_ptr();
		flag = pop_int();
		if (!savestr(0, dsp1) || (fd = creat(buf[0], flag)) == -1) {
			push_err();
			LOG(("@m4 Creat: failed, dsp1 = %lu, flag = %d, errno = %d",
					dsp1, flag, errno));
		}
		else {
			push_int(fd);
			push_int(0);
			LOG(("@m9 Creat: succeeded, fd = %d", fd));
		}
		break;

	case 9:				/* Link */

		dsp1 = pop_ptr();
		dsp2 = pop_ptr();
		if (	!savestr(0, dsp1)
		||	!savestr(1, dsp2)
		||	link(buf[0], buf[1]) == -1
		) {
			push_err();
			LOG(("@m4 Link: failed, dsp1 = %lu, dsp2 = %lu, errno = %d",
					dsp1, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Link: succeeded, dsp1 = %lu, dsp2 = %lu",
					dsp1, dsp2));
		}
		break;

	case 10:			/* Unlink */

		dsp1 = pop_ptr();
		if (!savestr(0, dsp1) || unlink(buf[0]) == -1) {
			push_err();
			LOG(("@m4 Unlink: failed, dsp1 = %lu, errno = %d",
					dsp1, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Unlink: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	case 12:			/* Chdir */

		dsp1 = pop_ptr();
		if (!savestr(0, dsp1) || chdir(buf[0]) == -1) {
			push_err();
			LOG(("@m4 Chdir: failed, dsp1 = %lu, errno = %d",
				dsp1, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Chdir: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	case 14:			/* Mknod */

		dsp1 = pop_ptr();
		mode = pop_int();
		address = pop_int2();
		if (!savestr(0, dsp1) || mknod(buf[0], mode, address) == -1) {
			push_err();
			LOG(("@m4 Mknod: failed, dsp1 = %lu, mode = %d, address = %d, errno = %d",
					dsp1, mode, address, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Mknod: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	case 15:			/* Chmod */

		dsp1 = pop_ptr();
		mode = pop_int2();
		if (!savestr(0, dsp1) || chmod(buf[0], mode) == -1) {
			push_err();
			LOG(("@m4 Chmod: failed, dsp1 = %lu, mode = %d, errno = %d",
				dsp1, mode, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Chmod: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	case 16:			/* Chown */

		dsp1 = pop_ptr();
		owner = pop_int2();
		group = pop_int2();
		if (!savestr(0, dsp1) || chown(buf[0], owner, group) == -1) {
			push_err();
			LOG(("@m4 Chown: failed, dsp1 = %lu, owner = %d, group = %d, errno = %d",
				dsp1, owner, group, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Chown: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	case 18:			/* Stat */

		dsp1 = pop_ptr();	/* points to file-name space */
		dsp2 = pop_ptr();	/* points to EM-stat-buffer space */
		if (	!savestr(0, dsp1)
		||	stat(buf[0], &st_buf) == -1
		||	!stat2mem(dsp2, &st_buf)
		) {
			push_err();
			LOG(("@m4 Stat: failed, dsp1 = %lu, dsp2 = %lu, errno = %d",
				dsp1, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Stat: succeeded, dsp1 = %lu, dsp2 = %lu",
				dsp1, dsp2));
		}
		break;

	case 19:			/* Lseek */

		fd = pop_int();
		off = pop_int4();
		whence = pop_int();
		LOG(("@m4 Lseek: fd = %d, off = %ld, whence = %d",
				fd, off, whence));

		if (!good_fd(fd) || (off = lseek(fd, off, whence)) == -1) {
			push_err();
			LOG(("@m9 Lseek: failed, errno = %d", errno));
		}
		else {
			push_i4(off);
			push_int(0);
			LOG(("@m9 Lseek: succeeded, pushed %ld", off));
		}
		break;

	case 20:			/* Getpid */

		pid = getpid();
		push_i2(pid);
		LOG(("@m9 Getpid: succeeded, pid = %d", pid));
		break;

	case 21:			/* Mount */

		dsp1 = pop_ptr();
		dsp2 = pop_ptr();
		flag = pop_int();
		if (	!savestr(0, dsp1)
		||	!savestr(1, dsp2)
		||	mount(buf[0], buf[1], flag) == -1
		) {
			push_err();
			LOG(("@m4 Mount: failed, dsp1 = %lu, dsp2 = %lu, flag = %d, errno = %d",
				dsp1, dsp2, flag, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Mount: succeeded, dsp1 = %lu, dsp2 = %lu, flag = %d",
				dsp1, dsp2, flag));
		}
		break;

	case 22:			/* Umount */

		dsp1 = pop_ptr();
		if (	!savestr(0, dsp1)
#ifndef	BSD4_2				/* from system.h */
		||	umount(buf[0]) == -1
#else	/* BSD4_2 */
		||	unmount(buf[0]) == -1
#endif	/* BSD4_2 */
		) {
			push_err();
			LOG(("@m4 Umount: failed, dsp1 = %lu, errno = %d",
				dsp1, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Mount: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	case 23:			/* Setuid */

		userid = pop_int2();
		if (setuid(userid) == -1) {
			push_err();
			LOG(("@m4 Setuid: failed, userid = %d, errno = %d",
				userid, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Setuid: succeeded, userid = %d", userid));
		}
		break;

	case 24:			/* Getuid */

		userid = getuid();
		push_i2(userid);
		LOG(("@m9 Getuid(part 1): real uid = %d", userid));
		userid = geteuid();
		push_i2(userid);
		LOG(("@m9 Getuid(part 2): eff uid = %d", userid));
		break;

	case 25:			/* Stime */

		tm = pop_int4();
#ifndef	BSD4_2				/* from system.h */
		rc = stime(&tm);
#else	/* BSD4_2 */
		tv.tv_sec = tm;
		tv.tv_usec = 0;		/* zero microseconds */
		rc = settimeofday(&tv, (struct timezone *)0);
#endif	/* BSD4_2 */
		if (rc == -1) {
			push_err();
			LOG(("@m4 Stime: failed, tm = %ld, errno = %d",
				tm, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Stime: succeeded, tm = %ld", tm));
		}
		break;

	case 26:			/* Ptrace */

		request = pop_int();
		pid = pop_int2();
		dsp3 = pop_ptr();
		n = pop_int();		/* Data */
		einval(WPTRACEIMP);
		push_err();
		LOG(("@m4 Ptrace: failed, request = %d, pid = %d, addr = %lu, data = %d, errno = %d",
			request, pid, dsp3, n, errno));
		break;

	case 27:			/* Alarm */

		seconds = pop_uns2();
		LOG(("@m9 Alarm(part 1) seconds = %u", seconds));
		seconds = alarm(seconds);
		push_i2(seconds);
		LOG(("@m9 Alarm(part 2) seconds = %u", seconds));
		break;

	case 28:			/* Fstat */

		fd = pop_int();
		dsp2 = pop_ptr();
		if (	!good_fd(fd)
		||	fstat(fd, &st_buf) == -1
		||	!stat2mem(dsp2, &st_buf)
		) {
			push_err();
			LOG(("@m4 Fstat: failed, fd = %d, dsp2 = %lu, errno = %d",
				fd, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Fstat: succeeded, fd = %d, dsp2 = %lu",
				fd, dsp2));
		}
		break;

	case 29:			/* Pause */

		pause();
		LOG(("@m9 Pause: succeeded"));
		break;

	case 30:			/* Utime */

		dsp1 = pop_ptr();
		dsp2 = pop_ptr();
		if (memfault(dsp2, 2*INT4SIZE)) {
			push_err();
			LOG(("@m4 Utime: failed, dsp1 = %lu, dsp2 = %lu, errno = %d",
					dsp1, dsp2, errno));
			break;
		}
		actime = mem_ldu(dsp2, INT4SIZE);
		modtime = mem_ldu(dsp2 + INT4SIZE, INT4SIZE);
#ifdef	SYS_V				/* from system.h */
		utimbuf.x = actime;
		utimbuf.y = modtime;
		if (!savestr(0, dsp1) || utime(buf[0], &utimbuf) == -1) {
#else	/* SYS_V */
		utimbuf[0] = actime;
		utimbuf[1] = modtime;
		if (!savestr(0, dsp1) || utime(buf[0], utimbuf) == -1) {
			/* may require modification for POSIX ???!!! */
#endif	/* !SYS_V */
			push_err();
			LOG(("@m4 Utime: failed, dsp1 = %lu, dsp2 = %lu, errno = %d",
					dsp1, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Utime: succeeded, dsp1 = %lu, dsp2 = %lu",
					dsp1, dsp2));
		}
		break;

	case 33:			/* Access */

		dsp1 = pop_ptr();
		mode = pop_int();
		if (!savestr(0, dsp1) || access(buf[0], mode) == -1) {
			push_err();
			LOG(("@m4 Access: failed, dsp1 = %lu, mode = %d, errno = %d",
					dsp1, mode, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Access: succeeded, dsp1 = %lu, mode = %d",
				dsp1, mode));
		}
		break;

	case 34:			/* Nice */

		incr = pop_int();
		nice(incr);
		LOG(("@m9 Nice: succeeded, incr = %d", incr));
		break;

	case 35:			/* Ftime */

		dsp2 = pop_ptr();
#ifdef	BSD_X				/* from system.h */
		ftime(&tb_buf);
#endif	/* BSD_X */
#ifdef	SYS_V				/* from system.h */
		tb_buf.time = time((time_t*)0);
		tb_buf.millitm = 0;
		tb_buf.timezone = timezone / 60;
		tb_buf.dstflag = daylight;
#endif	/* SYS_V */
		if (!timeb2mem(dsp2, &tb_buf)) {
			push_err();
			LOG(("@m4 Ftime: failed, dsp2 = %lu, errno = %d",
				dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Ftime: succeeded, dsp2 = %lu", dsp2));
		}
		break;

	case 36:			/* Sync */

		sync();
		LOG(("@m9 Sync: succeeded"));
		break;

	case 37:			/* Kill */

		pid = pop_int2();
		sig = pop_int();
		if (kill(pid, sig) == -1) {
			push_err();
			LOG(("@m4 Kill: failed, pid = %d, sig = %d, errno = %d",
					pid, sig, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Kill: succeeded, pid = %d, sig = %d",
				pid, sig));
		}
		break;

	case 41:			/* Dup & Dup2 */

		fd = pop_int();
		fdnew = pop_int();
		if (fd & DUPMASK) {
			int fd1 = fd & ~DUPMASK;/* stripped */

			LOG(("@m4 Dup2: fd1 = %d, fdnew = %d", fd1, fdnew));
			if (!good_fd(fd1)) {
				fdnew = -1;
				goto dup2_error;
			}
#ifdef	BSD_X				/* from system.h */
			fdnew = dup2(fd1, fdnew);
#endif	/* BSD_X */

#ifdef	SYS_V				/* from system.h */
			{
				/* simulating the semantics of dup2 on SYS_V */
				int dupped = dup(fd1);

				if (dupped < 0 && errno != EMFILE) {
					/*	the dup failed, but not
						because of too many open
						file descriptors
					*/
					fdnew = dupped;
				}
				else {
					close(dupped);
					close(fdnew);
					fdnew = fcntl(fd1, F_DUPFD, fdnew);
				}
			}
#endif	/* SYS_V */
		dup2_error:;
		}
		else {
			LOG(("@m4 Dup: fd = %d, fdnew = %d", fd, fdnew));
			fdnew = (!good_fd(fd) ? -1 : dup(fd));
		}

		if (fdnew == -1) {
			push_err();
			LOG(("@m4 Dup/Dup2: failed, fdnew = %d, errno = %d",
				fdnew, errno));
		}
		else {
			push_int(fdnew);
			push_int(0);
			LOG(("@m9 Dup/Dup2: succeeded, fdnew = %d", fdnew));
		}
		break;

	case 42:			/* Pipe */

		if (pipe(pfds) == -1) {
			push_err();
			LOG(("@m4 Pipe: failed, errno = %d", errno));
		}
		else {
			push_int(pfds[0]);
			push_int(pfds[1]);
			push_int(0);
			LOG(("@m9 Pipe: succeeded, pfds[0] = %d, pfds[1] = %d",
				pfds[0], pfds[1]));
		}
		break;

	case 43:			/* Times */

		dsp2 = pop_ptr();
		times(&tm_buf);
		if (!tms2mem(dsp2, &tm_buf)) {
			push_err();
			LOG(("@m4 Times: failed, dsp2 = %lu, errno = %d",
				dsp2, errno));
		}
		else {
			LOG(("@m9 Times: succeeded, dsp2 = %lu", dsp2));
		}
		break;

	case 44:			/* Profil */

		dsp1 = pop_ptr();	/* Buffer */
		nbytes = pop_intp();	/* Buffer size */
		off = pop_intp();	/* Offset */
		n = pop_intp();		/* Scale */
		einval(WPROFILIMP);
		push_err();
		LOG(("@m4 Profil: failed, dsp1 = %lu, nbytes = %d, offset = %d, scale = %d, errno = %d",
			dsp1, nbytes, off, n, errno));
		break;

	case 46:			/* Setgid */

		groupid = pop_int2();
		if (setgid(groupid) == -1) {
			push_err();
			LOG(("@m4 Setgid: failed, groupid = %d, errno = %d",
				groupid, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Setgid: succeeded, groupid = %d", groupid));
		}
		break;

	case 47:			/* Getgid */

		groupid = getgid();
		push_i2(groupid);
		LOG(("@m9 Getgid(part 1): succeeded, real groupid = %d",
				groupid));
		groupid = getegid();
		push_i2(groupid);
		LOG(("@m9 Getgid(part 2): succeeded, eff groupid = %d",
				groupid));
		break;

	case 48:			/* Sigtrp */

		trap_no = pop_int();
		sig_no = pop_int();
		
		if ((old_trap_no = do_sigtrp(trap_no, sig_no)) == -1) {
			push_err();
			LOG(("@m4 Sigtrp: failed, trap_no = %d, sig_no = %d, errno = %d",
					trap_no, sig_no, errno));
		}
		else {
			push_int(old_trap_no);
			push_int(0);
			LOG(("@m9 Sigtrp: succeeded, trap_no = %d, sig_no = %d, old_trap_no = %d",
					trap_no, sig_no, old_trap_no));
		}
		break;

	case 51:			/* Acct */

		dsp1 = pop_ptr();
		if (!savestr(0, dsp1) || acct(buf[0]) == -1) {
			push_err();
			LOG(("@m4 Acct: failed, dsp1 = %lu, errno = %d",
				dsp1, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Acct: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	case 54:			/* Ioctl */

		fd = pop_int();
		request = pop_int();
		dsp2 = pop_ptr();
		if (!good_fd(fd) || do_ioctl(fd, request, dsp2) != 0) {
			push_err();
			LOG(("@m4 Ioctl: failed, fd = %d, request = %d, dsp2 = %lu, errno = %d",
				fd, request, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Ioctl: succeeded, fd = %d, request = %d, dsp2 = %lu",
				fd, request, dsp2));
		}
		break;

	case 56:			/* Mpxcall */

		request = pop_int();	/* Command */
		dsp1 = pop_ptr();	/* Vec */
		einval(WMPXIMP);
		push_err();
		LOG(("@m4 Mpxcall: failed, request = %d, dsp1 = %lu, errno = %d",
			request, dsp1, errno));
		break;

	case 59:			/* Exec */

		dsp1 = pop_ptr();
		dsp2 = pop_ptr();
		dsp3 = pop_ptr();
		if (	!savestr(0, dsp1)
		||	!vec(1, 2, dsp2, &argvec)
		||	!vec(3, 4, dsp3, &envvec)
		||	/* execute results, ignore return code */
			(execve(buf[0], argvec, envvec), 1)
		) {
			push_err();
			LOG(("@m4 Exece: failed, dsp1 = %lu, dsp2 = %lu, dsp2 = %lu, errno = %d",
				dsp1, dsp2, dsp3, errno));
		}
		break;

	case 60:			/* Umask */

		mode = pop_int2();
		oldmask = umask(mode);
		push_int(oldmask);
		LOG(("@m9 Umask: succeeded, mode = %d, oldmask = %d",
			mode, oldmask));
		break;

	case 61:			/* Chroot */

		dsp1 = pop_ptr();
		if (!savestr(0, dsp1) || chroot(buf[0]) == -1) {
			push_err();
			LOG(("@m4 Chroot: failed, dsp1 = %lu, errno = %d",
				dsp1, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Chroot: succeeded, dsp1 = %lu", dsp1));
		}
		break;

	default:
		trap(EBADMON);
		break;
	}
}

/* Buffer administration */

PRIVATE check_buf(n, sz)
	int n;
	size sz;
{
	if (buf_cnt[n] == 0) {
		buf_cnt[n] = max(128, sz);
		buf[n] = Malloc(buf_cnt[n], "moncall buffer");
	}
	else if (buf_cnt[n] < sz) {
		buf_cnt[n] = allocfrac(sz);
		buf[n] = Realloc(buf[n], buf_cnt[n], "moncall buffer");
	}
}

PRIVATE int savestr(n, addr)
	int n;
	ptr addr;
{
	register size len;
	register char *cp, ch;

	/* determine the length, carefully */
	len = 0;
	do {
		if (memfault(addr + len, 1L)) {
			return 0;
		}
		ch = mem_loc(addr + len);
		len++;
	} while (ch);

	/* allocate enough buffer space */
	check_buf(n, len);

	/* copy the string */
	cp = buf[n];
	do {
		*cp++ = ch = mem_loc(addr);
		addr++;
	} while (ch);

	return 1;
}

PRIVATE int vec(n1, n2, addr, vecvec)
	int n1, n2;
	ptr addr;
	char ***vecvec;
{
	register char *cp1, *cp2;
	ptr p, ldp;
	register int n_ent = 0;		/* number of entries */
	register size str = 0;		/* total string length */

	/* determine number of elements n_ent */
	p = addr;
	do {
		if (memfault(addr, psize)) {
			return 0;
		}
		ldp = mem_lddp(p);
		if (!savestr(n2, ldp)) {
			return 0;
		}
		str += strlen(buf[n2]) + 1;
		n_ent++;
		p += psize;
	} while (ldp);
	n_ent++;

	*vecvec = (char **) Malloc((size)(n_ent * sizeof (char *)),
					"argvec or envvec in exec()");
	check_buf(n1, str);

	/* copy the elements */
	for (	cp1 = buf[n1], n_ent = 0, p = addr;
		ldp = mem_lddp(p);
		p += psize, n_ent++
	) {
		if (!savestr(n2, ldp)) {
			return 0;
		}
		(*vecvec)[n_ent] = cp1;
		cp2 = buf[n2];
		while (*cp1++ = *cp2++) {
			/* nothing */
		}
	}
	(*vecvec)[n_ent] = 0;
	return 1;
}

int memfault(addr, length)
	ptr addr;
	size length;
{
	/* centralizes (almost) all memory access tests in MON */
	if (!is_in_mem(addr, length)) {
		efault(WMONFLT);
		return 1;
	}
	return 0;
}

efault(wrn)
	int wrn;			/* warning number */
{
	warning(wrn);
	errno = 14;			/* EFAULT */
}

einval(wrn)
	int wrn;			/* warning number */
{
	warning(wrn);
	errno = 22;			/* EINVAL */
}

