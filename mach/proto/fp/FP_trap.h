/*
	include file for floating point package
*/

			/*	EM TRAPS	*/

#define	EIOVFL	3	/* Integer  Overflow		*/
#define	EFOVFL	4	/* Floating Overflow		*/
#define	EFUNFL	5	/* Floating Underflow		*/
#define	EIDIVZ	6	/* Integer  Divide by 0		*/
#define	EFDIVZ	7	/* Floating Divide by 0.0	*/
#define	EIUND	8	/* Integer  Undefined Number	*/
#define	EFUND	9	/* Floating Undefined Number	*/
#define	ECONV	10	/* Conversion Error		*/
# define trap(x) _fptrp(x)
