/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  O P E R A T I O N S    F O R
 *      C O M P A C T   S E T S
 */


extern	cset	Cempty_set();	/* (short)			*/
extern	bool	Cis_elem();	/* (Celem, cset)		*/
extern		Cadd();		/* (Celem, *cset)		*/
extern		Cremove();	/* (Celem, *cset)		*/
extern	Cindex	Cfirst();	/* (cset)			*/
extern	Cindex	Cnext();	/* (Cindex, cset)		*/
extern	Celem_t	Celem();	/* (Cindex)			*/
extern		Cjoin();	/* (cset, *cset)		*/
extern		Cintersect();	/* (cset, *cset)		*/
extern		Cdeleteset();	/* (cset)			*/
extern bool	Cis_subset();	/* (cset, cset)			*/
extern 		Cclearset();	/* (cset, *cset)		*/
extern		Ccopy_set();	/* (cset, *cset)		*/
extern		Csubtract();	/* (cset, *cset)		*/
extern bool	Cequal();	/* (cset, cset)			*/
extern	short	Cnrelems();	/* (cset)			*/
