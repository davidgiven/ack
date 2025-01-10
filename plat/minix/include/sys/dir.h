/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef _ACK_DIR_H
#define _ACK_DIR_H

#ifdef __BSD4_2
#define MAXNAMLEN 255
#else
#define MAXNAMLEN 14
#endif
#define DIRBLKSIZ 512
#undef DIRSIZ
#define DIRSIZ(dp) ((sizeof(struct direct) - (MAXNAMLEN + 1)) + (((dp)->d_namlen + 1 + 3) & ~3))
struct direct
{
	long d_ino;
	short d_reclen;
	short d_namlen;
	char d_name[MAXNAMLEN + 1];
};

struct _dirdesc
{
	int dd_fd;
	long dd_loc;
	long dd_size;
	char* dd_buf;
	int dd_bsize;
};

typedef struct _dirdesc DIR;

extern DIR* opendir(const char* name);
extern struct direct* readdir(DIR* dirp);
extern long telldir(DIR* dirp);
extern void seekdir(DIR* dirp, long loc);
#define rewinddir(dirp) seekdir((dirp), 0L)
extern int closedir(DIR* dirp);

#endif /* _DIR_H */
