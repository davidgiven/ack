/*
 * errno.h - errors
 */
/* $Header$ */

#if	!defined(_ERRNO_H)
#define	_ERRNO_H

#include	<sys/errno.h>

/* The standard requires the next two definitions. If they are also in
 * <sys/errno.h>, their values should be equal. The <sys/errno.h> supplied
 * with the compiler doesn't contain them.
 */
#define	EDOM		33	/* math arg out of domain of func */
#define	ERANGE		34	/* math result not representable */

extern int errno;		/* error number */

#endif	/* _ERRNO_H */
