/* $Id$ */

/*	As a starter, chars are divided into classes, according to which
	token they can be the start of.
*/

#define	class(ch)	(tkclass[ch])

#define	STSKIP	0	/* spaces and so on: skipped characters		*/
#define	STNL	1	/* newline character(s): update linenumber etc.	*/
#define	STGARB	2	/* garbage ascii character: not allowed		*/
#define	STDOT	3	/* '.' can start a number, or be a separate token */
#define	STCOMP	4	/* this one can start a compound token		*/
#define	STIDF	5	/* being the initial character of an identifier	*/
#define	STCHAR	6	/* the starter of a character constant		*/
#define	STSTR	7	/* the starter of a string			*/
#define	STNUM	8	/* the starter of a numeric constant		*/
#define	STEOI	9	/* End-Of-Information mark			*/
#define STSIMP  10      /* this character can occur as token            */

/*	But occurring inside a token is not an exclusive property,
	so we need 1 bit for each class. 
	This is implemented as a collection of tables to speed up
	the decision whether a character has a special meaning.
*/
#define	in_idf(ch)	((unsigned)ch < 0177 && inidf[ch])
#define	in_ext(ch)	((unsigned)ch < 0177 && inext[ch])
#define	is_oct(ch)	((unsigned)ch < 0177 && isoct[ch])
#define	is_dig(ch)	((unsigned)ch < 0177 && isdig[ch])
#define	is_hex(ch)	((unsigned)ch < 0177 && ishex[ch])
#define	is_token(ch)	((unsigned)ch < 0177 && istoken[ch])

extern char tkclass[];
extern char inidf[], isoct[], isdig[], ishex[], inext[], istoken[];
