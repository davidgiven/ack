/* System calls. */
#define SEND		   1	/* function code for sending messages */
#define RECEIVE		   2	/* function code for receiving messages */
#define BOTH		   3	/* function code for SEND + RECEIVE */
#define ANY   (NR_PROCS+100)	/* receive(ANY, buf) accepts from any source */

/* Task numbers, function codes and reply codes. */
#define HARDWARE          -1	/* used as source on interrupt generated msgs */

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
#ifdef ATARI_ST
#	define SYS_FRESH  10	/* fcn code for sys_fresh() */
#endif

#define CLOCK             -3	/* clock class */
#	define SET_ALARM   1	/* fcn code to CLOCK, set up alarm */
#	define CLOCK_TICK  2	/* fcn code for clock tick */
#	define GET_TIME	   3	/* fcn code to CLOCK, get real time */
#	define SET_TIME	   4	/* fcn code to CLOCK, set real time */
#	define REAL_TIME   1	/* reply from CLOCK: here is real time */

#define MEM               -4	/* /dev/ram, /dev/(k)mem and /dev/null class */
#	define RAM_DEV     0	/* minor device for /dev/ram */
#	define MEM_DEV     1	/* minor device for /dev/mem */
#	define KMEM_DEV    2	/* minor device for /dev/kmem */
#	define NULL_DEV    3	/* minor device for /dev/null */

#define FLOPPY            -5	/* floppy disk class */
#define WINCHESTER        -6	/* winchester (hard) disk class */
#	define DISKINT     1	/* fcn code for disk interupt */
#	define DISK_READ   3	/* fcn code to DISK (must equal TTY_READ) */
#	define DISK_WRITE  4	/* fcn code to DISK (must equal TTY_WRITE) */
#	define DISK_IOCTL  5	/* fcn code for setting up RAM disk */

#define TTY               -7	/* terminal I/O class */
#define PRINTER           -8	/* printer  I/O class */
#	define TTY_CHAR_INT 1	/* fcn code for tty input interrupt */
#	define TTY_O_DONE  2	/* fcn code for tty output done */
#	define TTY_READ	   3	/* fcn code for reading from tty */
#	define TTY_WRITE   4	/* fcn code for writing to tty */
#	define TTY_IOCTL   5	/* fcn code for ioctl */
#	define SUSPEND	-998	/* used in interrupts when tty has no data */

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
#define CHILD_STIME    m4_l4	/* system time consumed by proces children */

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
