#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#include <sys/types.h>

struct dirent {
	ino_t		d_ino;
	unsigned short	d_reclen;
	unsigned char	d_type;
	unsigned char	d_namlen;
#define MAXNAMLEN 255
	char		d_name[MAXNAMLEN + 1];
};

int getdirentries(int, char *, int, long *);

#endif
