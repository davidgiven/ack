/* $Header$ */

#if	!defined(_GRP_H)
#define	_GRP_H

struct	group {				/* see getgrent(3) */
	char	*gr_name;
	char	*gr_passwd;
#if	defined(_POSIX_SOURCE) && defined(_MINIX)
	char	gr_gid;
#else
	int	gr_gid;
#endif
	char	**gr_mem;
};

struct group *getgrent(void);
struct group *getgrgid(int __gid);
struct group *getgrnam(const char *__name);

#endif	/* _GRP_H */
