#define NCALLS            69  	/* number of system calls allowed */

#define EXIT	           1 
#define FORK	           2 
#define READ	           3 
#define WRITE	           4 
#define OPEN	           5 
#define CLOSE	           6 
#define WAIT	           7
#define CREAT	           8 
#define LINK	           9 
#define UNLINK	          10 
#define CHDIR	          12 
#define TIME	          13
#define MKNOD	          14 
#define CHMOD	          15 
#define CHOWN	          16 
#define BRK	          17
#define STAT	          18 
#define LSEEK	          19
#define GETPID	          20
#define MOUNT	          21 
#define UMOUNT	          22 
#define SETUID	          23
#define GETUID	          24
#define STIME	          25
#define ALARM	          27
#define FSTAT	          28 
#define PAUSE	          29
#define UTIME	          30 
#define ACCESS	          33 
#define SYNC	          36 
#define KILL	          37
#define DUP	          41 
#define PIPE	          42 
#define TIMES	          43
#define SETGID	          46
#define GETGID	          47
#define SIGNAL	          48
#define IOCTL             54
#define EXEC	          59
#define UMASK	          60 
#define CHROOT	          61 

/* The following are not system calls, but are processed like them. */
#define KSIG	          64	/* kernel detected a signal */
#define UNPAUSE           65	/* to MM or FS: check for EINTR */
#define BRK2	          66	/* to MM: used to say how big FS & INIT are */
#define REVIVE            67	/* to FS: revive a sleeping process */
#define TASK_REPLY        68	/* to FS: reply code from tty task */
