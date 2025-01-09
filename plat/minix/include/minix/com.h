/* System calls. */
#define SEND		   1	/* function code for sending messages */
#define RECEIVE		   2	/* function code for receiving messages */
#define BOTH		   3	/* function code for SEND + RECEIVE */
#define ANY   (NR_PROCS+100)	/* receive(ANY, buf) accepts from any source */

/* Task numbers, function codes and reply codes. */

#define TTY         -NR_TASKS	/* terminal I/O class */
#	define TTY_READ	    3	/* fcn code for reading from tty */
#	define TTY_WRITE    4	/* fcn code for writing to tty */
#	define TTY_IOCTL    5	/* fcn code for ioctl */
#	define TTY_SETPGRP  6	/* fcn code for setpgrp */
#	define TTY_OPEN     7	/* fcn code for opening tty */
#	define TTY_CLOSE    8	/* fcn code for closing tty */
#	define SUSPEND	 -998	/* used in interrupts when tty has no data */

#ifdef AM_KERNEL
#define	AMOEBA
#endif

#ifdef AMOEBA

/* There are AM_NTASK copies of the amoeba kernel task.
 * If you change AM_NTASKS be sure to adjust kernel/table.c and fs/table.c
 */
#define AM_NTASKS	   4	/* number of kernel tasks of this class */

#define	AMINT_CLASS	    (TTY+1)	/* Amoeba event handler */
#define AMOEBA_CLASS 	    (AMINT_CLASS+AM_NTASKS) /* transaction handlers */
#	define ETHER_ARRIV   1	/* fcn code for packet arrival */
#	define AM_TRANS      2	/* amoeba transaction */		
#	define AM_GETREQ     3	/* amoeba getrequest */
#	define AM_PUTREP     4	/* amoeba putrep */
#	define AM_REVIVE     6	/* used by kernel task to revive luser task */
#	define AM_TIMEOUT    8	/* used to talk to clock task */
#	define AM_PUTSIG     9	/* when the luser hits the DEL ! */
#	define AM_TASK_DIED 10  /* sent if task died during a transaction */

#else	/* if AMOEBA not defined */

#define	AMOEBA_CLASS	TTY

#endif /* AMOEBA */

/*
 * New class definitions should go here and should be defined relative
 * to AMOEBA_CLASS  (ie. as AMOEBA_CLASS+n, for the nth task added).
 */

#define IDLE (AMOEBA_CLASS+1)	/* task to run when there's nothing to run */

#define PRINTER           -7		/* printer  I/O class */
/* The printer uses the same commands as TTY. */

#define WINCHESTER        -6	/* winchester (hard) disk class */
#define FLOPPY            -5	/* floppy disk class */
#	define DISK_READ   3	/* fcn code to DISK (must equal TTY_READ) */
#	define DISK_WRITE  4	/* fcn code to DISK (must equal TTY_WRITE) */
#	define DISK_IOCTL  5	/* fcn code for setting up RAM disk */
#	define SCATTERED_IO 6	/* fcn code for multiple reads/writes */
#	define OPTIONAL_IO 16	/* modifier to DISK_* codes within vector */

#define MEM               -4	/* /dev/ram, /dev/(k)mem and /dev/null class */
#	define RAM_DEV     0	/* minor device for /dev/ram */
#	define MEM_DEV     1	/* minor device for /dev/mem */
#	define KMEM_DEV    2	/* minor device for /dev/kmem */
#	define NULL_DEV    3	/* minor device for /dev/null */
#if (CHIP == INTEL)
#	define PORT_DEV    4	/* minor device for /dev/port */
#endif

#define CLOCK             -3	/* clock class */
#	define SET_ALARM   1	/* fcn code to CLOCK, set up alarm */
#	define GET_TIME	   3	/* fcn code to CLOCK, get real time */
#	define SET_TIME	   4	/* fcn code to CLOCK, set real time */
#	define REAL_TIME   1	/* reply from CLOCK: here is real time */

#define SYSTASK           -2	/* internal functions */
#	define SYS_XIT     1	/* fcn code for sys_xit(parent, proc) */
#	define SYS_GETSP   2	/* fcn code for sys_sp(proc, &new_sp) */
#	define SYS_SIG     3	/* fcn code for sys_sig(proc, sig) */
#	define SYS_FORK    4	/* fcn code for sys_fork(parent, child) */
#	define SYS_NEWMAP  5	/* fcn code for sys_newmap(procno, map_ptr) */
#	define SYS_COPY    6	/* fcn code for sys_copy(ptr) */
#	define SYS_EXEC    7	/* fcn code for sys_exec(procno, new_sp) */
#	define SYS_TIMES   8	/* fcn code for sys_times(procno, bufptr) */
#	define SYS_ABORT   9	/* fcn code for sys_abort() */
#	define SYS_FRESH  10	/* fcn code for sys_fresh()  (Atari only) */
#	define SYS_KILL   11	/* fcn code for sys_kill(proc, sig) */
#	define SYS_GBOOT  12	/* fcn code for sys_gboot(procno, bootptr) */
#	define SYS_UMAP   13	/* fcn code for sys_umap(procno, etc) */
#	define SYS_MEM    14	/* fcn code for sys_mem() */
#	define SYS_TRACE  15	/* fcn code for sys_trace(req,pid,addr,data) */

#define HARDWARE          -1	/* used as source on interrupt generated msgs*/

/* Names of message fields for messages to CLOCK task. */
#define DELTA_TICKS    m6_l1	/* alarm interval in clock ticks */
#define FUNC_TO_CALL   m6_f1	/* pointer to function to call */
#define NEW_TIME       m6_l1	/* value to set clock to (SET_TIME) */
#define CLOCK_PROC_NR  m6_i1	/* which proc (or task) wants the alarm? */
#define SECONDS_LEFT   m6_l1	/* how many seconds were remaining */

/* Names of message fields used for messages to block and character tasks. */
#define DEVICE         m2_i1	/* major-minor device */
#define PROC_NR        m2_i2	/* which (proc) wants I/O? */
#define COUNT          m2_i3	/* how many bytes to transfer */
#define POSITION       m2_l1	/* file offset */
#define ADDRESS        m2_p1	/* core buffer address */

/* Names of message fields for messages to TTY task. */
#define TTY_LINE       m2_i1	/* message parameter: terminal line */
#define TTY_REQUEST    m2_i3	/* message parameter: ioctl request code */
#define TTY_SPEK       m2_l1	/* message parameter: ioctl speed, erasing */
#define TTY_FLAGS      m2_l2	/* message parameter: ioctl tty mode */
#define TTY_PGRP       m2_i3    /* message parameter: process group */

/* Names of messages fields used in reply messages from tasks. */
#define REP_PROC_NR    m2_i1	/* # of proc on whose behalf I/O was done */
#define REP_STATUS     m2_i2	/* bytes transferred or error number */

/* Names of fields for copy message to SYSTASK. */
#define SRC_SPACE      m5_c1	/* T or D space (stack is also D) */
#define SRC_PROC_NR    m5_i1	/* process to copy from */
#define SRC_BUFFER     m5_l1	/* virtual address where data come from */
#define DST_SPACE      m5_c2	/* T or D space (stack is also D) */
#define DST_PROC_NR    m5_i2	/* process to copy to */
#define DST_BUFFER     m5_l2	/* virtual address where data go to */
#define COPY_BYTES     m5_l3	/* number of bytes to copy */

/* Field names for accounting, SYSTASK and miscellaneous. */
#define USER_TIME      m4_l1	/* user time consumed by process */
#define SYSTEM_TIME    m4_l2	/* system time consumed by process */
#define CHILD_UTIME    m4_l3	/* user time consumed by process' children */
#define CHILD_STIME    m4_l4	/* sys time consumed by process' children */

#define PROC1          m1_i1	/* indicates a process */
#define PROC2          m1_i2	/* indicates a process */
#define PID            m1_i3	/* process id passed from MM to kernel */
#define STACK_PTR      m1_p1	/* used for stack ptr in sys_exec, sys_getsp */
#define PR             m6_i1	/* process number for sys_sig */
#define SIGNUM         m6_i2	/* signal number for sys_sig */
#define FUNC           m6_f1	/* function pointer for sys_sig */
#define MEM_PTR        m1_p1	/* tells where memory map is for sys_newmap */
#define CANCEL             0    /* general request to force a task to cancel */
#define SIG_MAP        m1_i2	/* used by kernel for passing signal bit map */

#ifdef AMOEBA

/* Names of message fields for amoeba tasks */
#define	AM_OP		m2_i1	/* one of the above operators */
#define	AM_PROC_NR	m2_i2	/* process # of proc doing operation */
#define	AM_COUNT	m2_i3	/* size of buffer for operation */
#define	AM_ADDRESS	m2_p1	/* address of buffer for operation */

/* For communication between MM and AMOEBA_CLASS kernel tasks */
#define	AM_STATUS	m2_i3	/* same use as REP_STATUS but for amoeba */
#define	AM_FREE_IT	m2_l1	/* 1=not a getreq, 0=is a getreq */

/* Special for passing a physical address from the ethernet driver */
#define	AM_PADDR	m2_l1	/* to the transaction layer */

#endif /* AMOEBA */

#define HARD_INT           2	/* fcn code for all hardware interrupts */
