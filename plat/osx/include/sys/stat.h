#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <sys/types.h>
#include <sys/time.h> /* for timespec */

struct stat {
	dev_t		st_dev;
	ino_t		st_ino;
	mode_t		st_mode;
	nlink_t		st_nlink;
	uid_t		st_uid;
	gid_t		st_gid;
	dev_t		st_rdev;
	struct timespec	st_atim;
	struct timespec	st_mtim;
	struct timespec	st_ctim;
#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec
	/*
	 * XXX - We don't have 64-bit integers, so we only expose the
	 * lower 32 bits of 64-bit fields.  We insert dummy fields for
	 * the higher 32 bits.
	 */
#if defined(__i386)
	off_t		st_size;
	off_t		_st_size_hi;
	blkcnt_t	st_blocks;
	blkcnt_t	_st_blkcnt_hi;
#elif defined(__powerpc)
	off_t		_st_size_hi;
	off_t		st_size;
	blkcnt_t	_st_blkcnt_hi;
	blkcnt_t	st_blkcnt;
#else
#error unknown arch
#endif
	blksize_t	st_blksize;
	unsigned int	st_flags;
	unsigned int	st_gen;
	unsigned int	_st_spare[5];
};

int fstat(int, struct stat *);
int lstat(const char *, struct stat *);
int stat(const char *, struct stat *);

#endif
