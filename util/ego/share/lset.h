/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  O P E R A T I O N S    F O R
 *	L O N G   S E T S
 */


extern	lset	Lempty_set();	/* ()				*/
extern	bool	Lis_elem();	/* (Lelem_t, lset)		*/
extern		Ladd();		/* (Lelem_t, *lset)		*/
extern		Lremove();	/* (Lelem_t, *lset)		*/
extern	Lindex	Lfirst();	/* (lset)			*/
extern	Lindex	Lnext();	/* (Lindex, lset)		*/
extern	Lelem_t	Lelem();	/* (Lindex)			*/
extern		Ljoin();	/* (lset, *lset)		*/
extern		Ldeleteset();	/* (lset)			*/
extern bool	Lis_subset();	/* (lset, lset)			*/
extern	short	Lnrelems();	/* (lset)			*/
