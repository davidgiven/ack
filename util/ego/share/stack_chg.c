/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T A C K _ C H A N G E . C */


#include <stdio.h>
#include <em_spec.h>
#include <em_mnem.h>
#include "types.h"
#include "debug.h"
#include "global.h"

#include "pop_push.h"

#define IS_LOC(l)	(l!=(line_p) 0 && INSTR(l)==op_loc && TYPE(l)==OPSHORT)

int stack_change(l,sign)
	line_p l;
	char sign;
{
	/* Interpret the string in the third column of the em_table file */

	char *s;
	bool argdef;
	short arg;
	int sum = 0;
	line_p p = PREV(l);
	line_p pp = (p == (line_p) 0 ? (line_p) 0 : PREV(p));
	short i = INSTR(l);

	if (i < sp_fmnem || i > sp_lmnem) {
		return 0;
	} else {
		if (TYPE(l) == OPSHORT) {
			arg = SHORT(l);
			if (arg < ws) {
				/* E.g. a LOI 1 loads word-size bytes,
				 * not 1 byte!
				 */
				arg = ws;
			}
			argdef = TRUE;
		} else {
			argdef = FALSE;
		}
	}
	s = pop_push[i];
	if (*s == '0') return 0;
	while (*s != '\0') {
		if (*s++ == sign) {
			switch(*s) {
				case 'w':
					sum +=  ws;
					break;
				case 'd':
					sum += 2 * ws;
					break;
				case 'p':
					sum += ps;
					break;
				case 'a':
					if (!argdef) return -1;
					sum +=  arg;
					break;
				case 'x':
					if (IS_LOC(p)) {
						sum += SHORT(p);
						break;
					} else {
						return -1;
					}
				case 'y':
					if (IS_LOC(pp)) {
						sum += SHORT(pp);
						break;
					} else {
						return -1;
					}
				case '?':
					return -1;
				default:
					assert(FALSE);
			}
		}
		s++;
	}
	return sum;
}



line_change(l,ok_out,pop_out,push_out)
	line_p l;
	bool *ok_out;
	int *pop_out,*push_out;
{
	short pop,push;

	pop = stack_change(l,'-');
	push = stack_change(l,'+');
	*ok_out = (pop != -1 && push != -1);
	*pop_out = pop;
	*push_out = push;
}


