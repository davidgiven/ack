#ifdef __CHANNEL__
#define	TIOCGETP	(('t'<<8)|8)
#define	TIOCSETN	(('t'<<8)|10)
struct sgttyb {
	char	sg_ispeed;
	char	sg_ospeed;
	char	sg_erase;
	char	sg_kill;
	short	sg_flags;
};

#define	CBREAK	02
#define	ECHO	010
#define	CRMOD	020
#else
#include "/usr/include/sgtty.h"
#endif
