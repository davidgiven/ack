/* The <sys/types.h> header contains important data type definitions.
 * It is considered good programming practice to use these definitions, 
 * instead of the underlying base type.  By convention, all type names end 
 * with _t.
 */

#ifndef _TYPES_H
#define _TYPES_H

/* The type size_t holds the result of the size_of operator.  This type is
 * 'unsigned int', in order to be compatible with the old library (f.i. the
 * argument of malloc was an unsigned int, and is now a size_t).  This means
 * that a 70K array can not be allocated.
 */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;	/* type returned by sizeof */
#endif

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;		/* time in sec since 1 Jan 1970 0000 GMT */
#endif

/* Types used in disk, inode, etc. data structures. */
typedef short	       dev_t;	/* holds (major|minor) device pair */
typedef char	       gid_t;	/* group id */
typedef unsigned short ino_t;	/* i-node number */
typedef short	      mode_t;	/* mode number within an i-node */
typedef char	     nlink_t;	/* number-of-links field within an i-node */
typedef long	       off_t;	/* offsets within a file */
typedef int	       pid_t;	/* type for pids (must be signed) */
typedef short	       uid_t;	/* user id */
typedef long	      zone_t;	/* holds a zone number */
typedef long	     block_t;	/* block number */
typedef long	       bit_t;	/* used for bit number in a bit map */

/* The following types are needed because MINIX uses K&R style function
 * definitions (for maximum portability).  When a short, such as dev_t, is
 * passed to a function with a K&R definition, the compiler automatically
 * promotes it to an int.  The prototype must contain an int as the parameter,
 * not a short, because an int is what an old-style function definition
 * expects.  Thus using dev_t in a prototype would be incorrect.  It would be
 * sufficient to just use int instead of dev_t in the prototypes, but Dev_t
 * is clearer.
 */
typedef int	Dev_t;
typedef int	Mode_t;

#endif /* _TYPES_H */
