/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/*	Lint class constants	*/

#define	LFDF	'a'	/* Library Function Definition */
#define	LVDF	'b'	/* Library Variable Definition */

#define	PFDF	'd'	/* Prototype Function Definition */

#define	EFDF	'f'	/* External Function Definition */
#define	EVDF	'g'	/* External Variable Definition */
#define	EFDC	'h'	/* External Function Declaration */
#define	EVDC	'i'	/* External Variable Declaration */

#define	IFDC	'm'	/* Implicit Function Declaration */

#define	SFDF	'q'	/* Static Function Definition */
#define	SVDF	'r'	/* Static Variable Definition */

#define	FC	'u'	/* Function Call */
#define	VU	'v'	/* Variable Usage */

#define	XXDF	'z'	/* Ignore Class */

/* Two meta-definitions */
#define	MIN_CLASS_CONST	LFDF
#define	MAX_CLASS_CONST	XXDF

