/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifdef	CHECK

public check_mallinks(const char *s), calc_checksum(mallink *ml);
public check_work_empty(const char *s);
public started_working_on(mallink *ml), stopped_working_on(mallink *ml);

#else	/* ifndef	CHECK */

#define	maldump(n)		abort()
#define	check_mallinks(s)	0
#define	calc_checksum(ml)	0
#define	started_working_on(ml)	0
#define	stopped_working_on(ml)	0
#define	check_work_empty(s)	0

#endif	/* CHECK */
