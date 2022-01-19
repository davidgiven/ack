#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <sys/types.h>

extern pid_t wait(int *__wstatus);
extern pid_t waitpid(pid_t __pid, int *__wstatus, int __options);

#define WNOHANG		(1 <<  0)
#define WUNTRACED	(1 <<  1)
#define WSTOPPED	WUNTRACED
#define WEXITED		(1 <<  2)
#define WCONTINUED	(1 <<  3)
#define WNOWAIT		(1 << 24)
#define __WNOTHREAD	(1 << 29)
#define __WALL		(1 << 30)
#define __WCLONE	(1 << 31)

#endif
