#ifdef __CHANNEL__
struct sgttyb {
	char	sg_ispeed;
	char	sg_ospeed;
	char	sg_erase;
	char	sg_kill;
	short	sg_flags;
};

#ifdef __BSD4_2
#define	TIOCGETP	((('t'<<8)|8)|(sizeof(struct sgttyb)<<16))
#define	TIOCSETN	((('t'<<8)|10)|(sizeof(struct sgttyb)<<16))
#else
#define	TIOCGETP	(('t'<<8)|8)
#define	TIOCSETN	(('t'<<8)|10)
#endif

#define	CBREAK	02
#define	ECHO	010
#define	CRMOD	020
#else
#include "/usr/include/sgtty.h"
#endif
