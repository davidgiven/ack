/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifdef	CHECK

public check_mallinks(), calc_checksum(), check_work_empty();
public started_working_on(), stopped_working_on();

#else	/* ifndef	CHECK */

#define	maldump(n)		abort()
#define	check_mallinks(s)	0
#define	calc_checksum(ml)	0
#define	started_working_on(ml)	0
#define	stopped_working_on(ml)	0
#define	check_work_empty(s)	0

#endif	/* CHECK */
