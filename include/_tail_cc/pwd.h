/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef _PWD_H
#define _PWD_H

struct passwd {
  char *pw_name;		/* login name */
  int pw_uid;			/* uid corresponding to the name */
  int pw_gid;			/* gid corresponding to the name */
  char *pw_dir;			/* user's home directory */
  char *pw_shell;		/* name of the user's shell */

  /* The following members are not defined by POSIX. */
  char *pw_passwd;		/* password information */
  char *pw_gecos;		/* just in case you have a GE 645 around */
};

extern struct passwd *getpwnam();
extern struct passwd *getpwuid();
extern struct passwd *getpwent();

#endif /* _PWD_H */
