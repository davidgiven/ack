/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* U S E   O F   C H A R A C T E R   C L A S S E S */

/*	As a starter, chars are divided into classes, according to which
	token they can be the start of.
	At present such a class number is supposed to fit in 4 bits.
*/

#define	class(ch)	(tkclass[(unsigned char) ch])

/*	Being the start of a token is, fortunately, a mutual exclusive
	property, so, as there are less than 16 classes they can be
	packed in 4 bits.
*/

#define	STSKIP	0	/* spaces and so on: skipped characters		*/
#define	STNL	1	/* newline character(s): update linenumber etc.	*/
#define	STGARB	2	/* garbage ascii character: not allowed		*/
#define	STSIMP	3	/* this character can occur as token		*/
#define	STCOMP	4	/* this one can start a compound token		*/
#define	STIDF	5	/* being the initial character of an identifier	*/
#define	STCHAR	6	/* the starter of a character constant		*/
#define	STSTR	7	/* the starter of a string			*/
#define	STNUM	8	/* the starter of a numeric constant		*/
#define	STEOI	9	/* End-Of-Information mark			*/

/*	But occurring inside a token is not, so we need 1 bit for each
	class.
*/
#define _I_	01
#define _O_	02
#define _D_	04
#define _H_	010

#define	in_idf(ch)	(tk2class[(unsigned char)ch] & _I_)
#define	is_oct(ch)	(tk2class[(unsigned char)ch] & _O_)
#define	is_dig(ch)	(tk2class[(unsigned char)ch] & _D_)
#define	is_hex(ch)	(tk2class[(unsigned char)ch] & _H_)

extern char tkclass[], tk2class[];
