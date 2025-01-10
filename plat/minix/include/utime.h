/* The <utime.h> header is used for the utime() system call. */

#ifndef _UTIME_H
#define _UTIME_H

struct utimbuf
{
	time_t actime; /* access time */
	time_t modtime; /* modification time */
};

extern int utime(const char* _path, const struct utimbuf* _times);

#endif /* _UTIME_H */
