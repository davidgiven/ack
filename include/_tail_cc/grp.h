/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef _GRP_H
#define _GRP_H

struct	group { 
  char *gr_name;		/* the name of the group */
  char *gr_passwd;		/* the group passwd */
  int gr_gid;			/* the numerical group ID */
  char **gr_mem;		/* a vector of pointers to the members */
};

extern struct group *getgrgid();
extern struct group *getgrnam();
extern struct group *getgrent();

#endif /* _GRP_H */
