#ifndef _ACK_FCNTL_H
#define _ACK_FCNTL_H

/* Linux O_ constants. */

enum
{
	O_ACCMODE = 0x3,
	
	O_RDONLY = 0,
	O_WRONLY = 1,
	O_RDWR = 2,
	
	O_CREAT  = 00000100,
	O_TRUNC  = 00001000,
	O_APPEND = 00002000
};

#endif
