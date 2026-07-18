#ifndef _ACK_FCNTL_H
#define _ACK_FCNTL_H

enum
{
	O_ACCMODE = 0x3,

	O_RDONLY = 0,
	O_WRONLY = 1,
	O_RDWR = 2,

	O_APPEND = 0x0008,
	O_CREAT  = 0x0100,
	O_TRUNC  = 0x0200,
	O_EXCL   = 0x0400,
	O_NOCTTY = 0x0800
};

#endif
