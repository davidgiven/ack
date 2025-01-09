/* The <utime.h> header is used for the utime() system call. */

#ifndef _UTIME_H
#define _UTIME_H

struct utimbuf {
  time_t actime;		/* access time */
  time_t modtime;		/* modification time */
};


/* Function Prototypes. */
#ifndef _ANSI_H
#include <ansi.h>
#endif

_PROTOTYPE( int utime, (char *_path, struct utimbuf *_times)		);

#endif /* _UTIME_H */
