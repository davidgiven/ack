/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * Basic system types and major/minor device constructing/busting macros.
 */
/* $Header$ */

#if	!defined(_SYS_TYPES_H)
#define	_SYS_TYPES_H

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
#if	defined(__BSD4_2)
typedef	u_long ino_t;
#else
typedef	u_short ino_t;
#endif
typedef	long	swblk_t;

#if	!defined(_SIZE_T)
#define	_SIZE_T
#if	_EM_WSIZE  == _EM_PSIZE
typedef unsigned int	size_t;		/* type returned by sizeof */
#else
typedef unsigned long	size_t;		/* type returned by sizeof */
#endif
#endif	/* _SIZE_T */

#if	!defined(_TIME_T)
#define	_TIME_T
typedef	unsigned long	time_t;
#endif	/* TIME_T */

typedef	long	label_t[14];
typedef	short	dev_t;
#if	defined(__BSD4_2)
typedef	int	off_t;			/* see lseek(2) */
#else
typedef	long	off_t;
#endif

/* system V compatibility: */
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned char	uchar_t;
typedef short		cnt_t;
typedef long		paddr_t;
typedef long		key_t;

#endif	/* _SYS_TYPES_H */
