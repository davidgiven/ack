/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* PREPROCESSOR: SCANNER FOR THE ACTUAL PARAMETERS OF MACROS	*/

/*	This file contains the function getactuals() which scans an actual
	parameter list and splits it up into a list of strings, each one
	representing an actual parameter.
*/

#include	"lapbuf.h"	/* UF */
#include	"nparams.h"	/* UF */

#include	"input.h"
#include	"class.h"
#include	"idf.h"
#include	"macro.h"
#include	"interface.h"
#include	"file_info.h"

#define	EOS		'\0'
#define	overflow()	(fatal("actual parameter buffer overflow"))

PRIVATE char apbuf[LAPBUF]; /* temporary storage for actual parameters	*/
PRIVATE char *actparams[NPARAMS]; /* pointers to the text of the actuals */
PRIVATE char *aptr;	/* pointer to last inserted character in apbuf	*/

#define	copy(ch)	((aptr < &apbuf[LAPBUF]) ? (*aptr++ = ch) : overflow())

PRIVATE int nr_of_params;	/* number of actuals read until now	*/

PRIVATE char **
getactuals(idef)
	struct idf *idef;
{
	/*	getactuals() collects the actual parameters and turns them
		into a list of strings, a pointer to which is returned.
	*/
	register acnt = idef->id_macro->mc_nps;

	nr_of_params = 0;
	actparams[0] = aptr = &apbuf[0];
	copyact('(', ')', 0);	/* read the actual parameters	*/
	copy(EOS);		/* mark the end of it all	*/

	if (!nr_of_params++)	{		/* 0 or 1 parameter	*/
		/* there could be a ( <spaces, comment, ...> )
		*/
		register char *p = actparams[0];

		while ((class(*p) == STSKIP) || (*p == '\n')) {
				++p;
		}

		if (!*p) {	/* the case () : 0 parameters	*/
			nr_of_params--;
		}
	}

	if (nr_of_params != acnt)	{
		/*	argument mismatch: too many or too few
			actual parameters.
		*/
		warning("argument mismatch, %s", idef->id_text);

		while (nr_of_params < acnt) {
			/*	too few paraeters: remaining actuals are ""
			*/
			actparams[nr_of_params] = "";
			nr_of_params++;
		}
	}

	return actparams;
}

PRIVATE
copyact(ch1, ch2, level)
	char ch1, ch2;
	int level;
{
	/*	copyact() is taken from Ceriel Jacobs' LLgen, with
		permission.  Its task is to build a list of actuals
		parameters, which list is surrounded by '(' and ')' and in
		which the parameters are separated by ',' if there are
		more than 1. The balancing of '(',')' and '[',']' and
		'{','}' is taken care of by calling this function
		recursively. At each level, copyact() reads the input,
		upto the corresponding closing bracket.

		Opening bracket is ch1, closing bracket is ch2. If
		level != 0, copy opening and closing parameters too.
	*/
	register int ch;		/* Current char */
	register int match;		/* used to read strings */

	if (level) {
		copy(ch1);
	}

	for (;;)	{
		LoadChar(ch);

		if (ch == ch2)	{
			if (level) {
				copy(ch);
			}
			return;
		}

		switch(ch)	{

#ifdef __MATCHING_PAR__
		case ')':
		case '}':
		case ']':
			error("unbalanced parenthesis");
			break;
#endif /* __MATCHING_PAR__ */

		case '(':
			copyact('(', ')', level+1);
			break;

#ifdef __MATCHING_PAR__
		case '{':
			/*	example:
					#define declare(v, t)	t v
					declare(v, union{int i, j; float r;});
			*/
			copyact('{', '}', level+1);
			break;

		case '[':
			copyact('[', ']', level+1);
			break;
#endif /* __MATCHING_PAR__ */

		case '\n':
			LineNumber++;
			LoadChar(ch);
			while (ch == '#')	{
				/*	This piece of code needs some
					explanation: consider the call of
					the macro defined as:
						#define sum(b,c) (b + c)
					in the following form:
						sum(
						#include my_phone_number
						,2)
					in which case the include must be
					interpreted as such.
				*/
				domacro();	/* has read nl, vt or ff */
				LoadChar(ch);
				/* Loop, for another control line */
			}

			PushBack();
			copy(' ');
			break;

		case '/':
			LoadChar(ch);

			if (ch == '*')	{	/* skip comment	*/
				skipcomment();
				continue;
			}

			PushBack();
			copy('/');
			break;

		case ',':
			if (!level)	{
				/* next parameter encountered */
				copy(EOS);

				if (++nr_of_params >= NPARAMS) {
					fatal("(getact) too many actuals");
				}

				actparams[nr_of_params] = aptr;
			}
			else	{
				copy(ch);
			}
			break;

		case '\'':
		case '"' :
			/*	watch out for brackets in strings, they do
				not count !
			*/
			match = ch;
			copy(ch);
			LoadChar(ch);
			while (ch != EOI)	{
				if (ch == match) {
					break;
				}

				if (ch == '\\')	{
					copy(ch);
					LoadChar(ch);
				}
				else
				if (ch == '\n')	{
					LineNumber++;
					error("newline in string");
					copy(match);
					break;
				}

				copy(ch);
				LoadChar(ch);
			}

			if (ch == match)	{
				copy(ch);
				break;
			}
			/* Fall through */

		case EOI :
			error("unterminated macro call");
			return;

		default:
			copy(ch);
			break;
		}
	}
}
