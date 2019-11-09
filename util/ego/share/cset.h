/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  O P E R A T I O N S    F O R
 *      C O M P A C T   S E T S
 */


cset	Cempty_set(short);
bool	Cis_elem(Celem_t, cset);
void	Cadd(Celem_t, cset *);
void	Cremove(Celem_t, cset *);
Cindex	Cfirst(cset);
Cindex	Cnext(Cindex, cset);
Celem_t	Celem(Cindex);
void	Cjoin(cset, cset *);
void	Cintersect(cset, cset *);
void	Cdeleteset(cset);
bool	Cis_subset(cset, cset);
void	Cclear_set(cset *);
void	Ccopy_set(cset, cset *);
void	Csubtract(cset, cset *);
bool	Cequal(cset, cset);
short	Cnrelems(cset);
