#ifdef __CHANNEL__
#define	NCC	8

#define	VMIN	4
#define	VTIME	5

#define	ICRNL	0000400

#define	ONLCR	0000004

#define	ICANON	0000002
#define	ECHO	0000010

struct termio {
	unsigned short	c_iflag;
	unsigned short	c_oflag;
	unsigned short	c_cflag;
	unsigned short	c_lflag;
	char	c_line;
	unsigned char	c_cc[NCC];
};

#define	TIOC	('T'<<8)
#define	TCGETA	(TIOC|1)
#define	TCSETA	(TIOC|2)
#else
#include "/usr/include/termio.h"
#endif
