/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/********************************************************/
/*
	include file for floating point package
*/
/********************************************************/
/*
	THESE STRUCTURES ARE USED TO ADDRESS THE INDIVIDUAL
	PARTS OF THE FLOATING NUMBER REPRESENTATIONS.

	THREE STRUCTURES ARE DEFINED:
		SINGLE:	single precision floating format
		DOUBLE:	double precision floating format
		EXTEND:	double precision extended format
*/
/********************************************************/
typedef	unsigned long	_float;

typedef	union	{
	double		_dbl;
	unsigned long	__double[2];
}	_double;

typedef	union	{	/* address parts of float	*/
		/* field to extract exponent	*/
	short	sgl_exp[sizeof(float)/sizeof(short)];
		/* same as fract below		*/
	_float	f[sizeof(float)/sizeof(_float)];
	unsigned long	fract;
} SINGLE;

typedef	union	{
		/* field to extract exponent	*/
	short	dbl_exp[sizeof(double)/sizeof(short)];
		/* for normal syntax use */
	_double	_f8[sizeof(double)/sizeof(_double)];
		/* a float in a double - for returns */
	_float	_f4[sizeof(double)/sizeof(_float)];
		/* to be deleted eventually	*/
	struct	{	/* address parts of float	*/
		SINGLE	p1;	/* part one */
		unsigned long	p2;	/* part two */
	} _s;
} DOUBLE;

typedef	struct	{	/* expanded float format	*/
	short	sign;
	short	exp;
	unsigned long	m1;
	unsigned long	m2;	/* includes guard byte	*/
} EXTEND;
#ifdef	PRT_EXT
#include <stdio.h>
#endif
