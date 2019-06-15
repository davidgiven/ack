/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef __ARCH_H_INCLUDED
#define __ARCH_H_INCLUDED

#define	ARMAG	0177545
#define AALMAG	0177454
#define AR_NAME_MAX 14

struct	ar_hdr {
	/** null terminated filename. */
	char	ar_name[AR_NAME_MAX];
	/** last modification time, defined as seconds since epoch. */
	long	ar_date;
	char	ar_uid;
	char	ar_gid;
	short	ar_mode;
	long	ar_size;
};

#define AR_TOTAL	26
#define AR_SIZE		22


/** ar_mode Read permission bit for the owner of the file. */
#define AR_IRUSR	0400
/** ar_mode Write permission bit for the owner of the file. */
#define AR_IWUSR	0200
/** ar_mode Execute (for ordinary files) or search (for directories)
 *  permission bit for the owner of the file.
 */
#define AR_IXUSR	0100
/** ar_mode Read permission bit for the group owner of the file. */
#define AR_IRGRP	040
/** ar_mode Write permission bit for the group owner of the file. */
#define AR_IWGRP	020
/** ar_mode Execute or search permission bit for the group owner of the file. */
#define AR_IXGRP	010
/** ar_mode Read permission bit for other users. */
#define AR_IROTH	04
/** ar_mode Write permission for other users. */
#define AR_IWOTH	02
/** ar_mode Execute or search permission bit for other users. */
#define AR_IXOTH	01

/* This is the set-user-ID on execute bit. */
#define	AR_ISUID	04000
/* This is the set-group-ID on execute bit. */
#define	AR_ISGID	02000



#endif /* __ARCH_H_INCLUDED */
