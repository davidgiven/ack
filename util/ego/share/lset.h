/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  O P E R A T I O N S    F O R
 *	L O N G   S E T S
 */


lset	Lempty_set(void);
bool	Lis_elem(Lelem_t, lset);
void	Ladd(Lelem_t, lset *);
void	Lremove(Lelem_t, lset *);
Lindex	Lfirst(lset);
Lindex	Lnext(Lindex, lset);
Lelem_t	Lelem(Lindex);
void	Ljoin(lset, lset *);
void	Ldeleteset(lset);
bool	Lis_subset(lset, lset);
short	Lnrelems(lset);
