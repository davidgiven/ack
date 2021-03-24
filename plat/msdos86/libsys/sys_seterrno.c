/* $Source$
 * $State$
 * $Revision$
 */

#include <errno.h>

static const signed char err_map[] =
	{
		0,			/* 0x00 no error */
		EINVAL,			/* 0x01 function number invalid */
		ENOENT,			/* 0x02 file not found */
		ENOENT,			/* 0x03 path not found */
		EMFILE,			/* 0x04 too many open files */
		EACCES,			/* 0x05 access denied */
		EBADF,			/* 0x06 invalid handle */
		EFAULT,			/* 0x07 mem. control block destroyed */
		ENOMEM,			/* 0x08 insufficient memory */
		EFAULT,			/* 0x09 memory block address invalid */
		E2BIG,			/* 0x0A environment invalid */
		ENOEXEC,		/* 0x0B format invalid */
		EINVAL,			/* 0x0C access code invalid */
		EINVAL,			/* 0x0D data invalid */
		ENOEXEC,		/* 0x0E (?) fixup overflow */
		ENODEV,			/* 0x0F invalid drive */
		EBUSY,			/* 0x10 attempt to remove curr. dir. */
		EXDEV,			/* 0x11 not same device */
		ENOENT,			/* 0x12 no more files */
		EIO,			/* 0x13 disk write-protected */
		EIO,			/* 0x14 unknown unit */
		ENXIO,			/* 0x15 drive not ready */
	};

/*
 * Map an MS-DOS 2+ system error code to an `errno' value and store that in
 * `errno'.  Return -1.
 */
int _sys_seterrno(unsigned dos_err)
{
	if (dos_err < sizeof(err_map) / sizeof(err_map[0]))
		errno = err_map[dos_err];
	else
		errno = EIO;
	return -1;
}
