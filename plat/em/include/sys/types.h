#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <stddef.h> /* for off_t, ptrdiff_t, size_t */

typedef int			blkcnt_t; /* XXX should have 64 bits */
typedef int			blksize_t;
typedef int			dev_t;
typedef unsigned int		gid_t;
typedef unsigned int		ino_t;
typedef unsigned short		mode_t;
typedef unsigned short		nlink_t;
typedef int			pid_t;
typedef unsigned int		uid_t;
typedef long time_t;
typedef int suseconds_t;

#endif
