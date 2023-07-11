#ifndef _ACK_CONFIG_H
#define _ACK_CONFIG_H

#include <ack/plat.h>

/* These configure how the stdlib is built. */

#ifndef ACKCONF_WANT_FLOAT
#define ACKCONF_WANT_FLOAT 1
#endif

#ifndef ACKCONF_WANT_STDIO_FLOAT
#define ACKCONF_WANT_STDIO_FLOAT ACKCONF_WANT_FLOAT
#endif

#ifndef ACKCONF_WANT_STANDARD_O
#define ACKCONF_WANT_STANDARD_O 1
#endif

#ifndef ACKCONF_WANT_O_TEXT_O_BINARY
#define ACKCONF_WANT_O_TEXT_O_BINARY 0
#endif

#ifndef ACKCONF_WANT_STANDARD_SIGNALS
#define ACKCONF_WANT_STANDARD_SIGNALS 1
#endif

#ifndef ACKCONF_WANT_TERMIOS
/* Don't compile termios-using functions unless the plat explicitly asks for it. */
#define ACKCONF_WANT_TERMIOS 0
#endif

#ifndef ACKCONF_WANT_EMULATED_RAISE
/* Implement raise() in terms of kill() and getpid(). */
#define ACKCONF_WANT_EMULATED_RAISE 1
#endif

#ifndef ACKCONF_WANT_EMULATED_REMOVE
/* Implement remove() as unlink(). */
#define ACKCONF_WANT_EMULATED_REMOVE 1
#endif

#ifndef ACKCONF_WANT_EMULATED_SYSTEM
/* Implement system() as fork()/execve()/wait(). */
#define ACKCONF_WANT_EMULATED_SYSTEM 1
#endif

#ifndef ACKCONF_WANT_EMULATED_SLEEP
/* Implement sleep() with SIGALRM. */
#define ACKCONF_WANT_EMULATED_SLEEP 1
#endif

#ifndef ACKCONF_WANT_EMULATED_POPEN
/* Implement popen() with fork()/dup2() etc. */
#define ACKCONF_WANT_EMULATED_POPEN 1
#endif

#ifndef ACKCONF_WANT_EMULATED_TIME
/* Implement time() with gettimeofday(). */
#define ACKCONF_WANT_EMULATED_TIME 1
#endif

#ifndef ACKCONF_WANT_MALLOC
/* Uses sbrk() to get memory from the system. */
#define ACKCONF_WANT_MALLOC 1
#endif

#ifndef ACKCONF_WANT_STDIO
#define ACKCONF_WANT_STDIO 1
#endif

#ifndef ACKCONF_WANT_EMULATED_FILE
/* Implement FILE* on top of read()/write() file descriptors. */
#define ACKCONF_WANT_EMULATED_FILE 1
#endif

/* These are tested to report features of the system. */

#ifndef ACKCONF_HAS_BOUNDS_CHECKING
/* The code generator supports bounds checking in the rck, aar, lar and sar instructions. */
#define ACKCONF_HAS_BOUNDS_CHECKING 1
#endif

#endif
