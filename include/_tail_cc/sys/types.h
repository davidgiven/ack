/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef __TYPES_H__
#define __TYPES_H__

/*
 * Basic system types and major/minor device constructing/busting macros.
 */

/* major part of a device */
#define	major(x)	((int)(((unsigned)(x)>>8)&0377))

/* minor part of a device */
#define	minor(x)	((int)((x)&0377))

/* make a device number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;

typedef	struct	_physadr { int r[1]; } *physadr;
typedef	long	daddr_t;
typedef	char *	caddr_t;
#ifdef __BSD4_2
typedef	u_long ino_t;
#else
typedef	u_short ino_t;
#endif
typedef	long	swblk_t;

#include <sys/stdtypes.h>

typedef	long	label_t[14];
typedef	short	dev_t;
typedef	long	off_t;

/* system V compatibility: */
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned char	uchar_t;
typedef short		cnt_t;
typedef long		paddr_t;
typedef long		key_t;

#endif /* __TYPES_H__ */
