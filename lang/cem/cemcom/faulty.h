/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* FAULTY DEFINITIONS */

#define	faulty(tp)	((tp)_faulty(__FILE__, __LINE__))
#define	fault()		(_faulty(__FILE__, __LINE__))
