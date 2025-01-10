#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

typedef long time_t;
typedef long suseconds_t;

/* Types used in disk, inode, etc. data structures. */
typedef short dev_t; /* holds (major|minor) device pair */
typedef char gid_t; /* group id */
typedef unsigned short ino_t; /* i-node number */
typedef short mode_t; /* mode number within an i-node */
typedef char nlink_t; /* number-of-links field within an i-node */
typedef int pid_t; /* type for pids (must be signed) */
typedef short uid_t; /* user id */
typedef long zone_t; /* holds a zone number */
typedef long block_t; /* block number */
typedef long bit_t; /* used for bit number in a bit map */

#endif
