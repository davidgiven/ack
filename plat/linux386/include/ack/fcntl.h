#ifndef _ACK_FCNTL_H
#define _ACK_FCNTL_H

/* Linux O_ constants. */

enum
{
	O_ACCMODE = 0x3,
	
	O_RDONLY = 0,
	O_WRONLY = 1,
	O_RDWR = 2,
	
	O_CREAT = 0x40,
	O_TRUNC = 0x200,
	O_APPEND = 0x400
};

#endif
