/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdlib.h>
#include <string.h>
#include "ack.h"
#include "list.h"
#include "trans.h"
#include "grows.h"
#include "data.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$";
static char rcs_trans[] = RCS_TRANS;
#endif

/****************************************************************************/
/*      Routines for transforming from one file type to another             */
/****************************************************************************/

static growstring head;
static int touch_head = NO;
static growstring tail;
static int touch_tail = NO;

static char* headvar(void);
static char* tailvar(void);
static void set_Rflag(char*);
static void condit(growstring*, list_head*, list_head*, char*);
static int mapflag(list_head*, const char*);
static int mapexpand(char*, const char*);
static void getcallargs(trf*);
static growstring without_bslash(const char*);
static void getprogram(trf*);

int transform(trf* phase)
{
	int ok;

	if (!setfiles(phase))
	{
		disc_files(phase);
		return 0;
	}
	getcallargs(phase);
	getprogram(phase);
	ok = runphase(phase);
	if (!ok)
		rmfile(&out);
	/* Free the space occupied by the arguments,
	   except for the linker, since we are bound to exit soon
	   and do not foresee further need of memory space */
	if (!phase->t_linker)
		l_throw(&phase->t_args);
	disc_files(phase);
	return ok;
}

void getmapflags(trf* phase)
{
	register path* l_in;
	register list_elem* elem;
	int scanned;
	register char* ptr;

	scanlist(l_first(flags), elem)
	{
		scanned = *(l_content(*elem)) & NO_SCAN;
		*(l_content(*elem)) &= ~NO_SCAN;
		if (mapflag(&(phase->t_mapf), l_content(*elem)))
		{
			scanned = NO_SCAN;
#ifdef DEBUG
			if (debug >= 4)
			{
				vprint("phase %s, added mapflag for %s\n", phase->t_name, l_content(*elem));
			}
#endif
		}
		*(l_content(*elem)) |= scanned;
	}
	if (phase->t_linker)
	{
		scanlist(l_first(phase->t_inputs), elem)
		{
			l_in = p_cont(*elem);
			if (mapflag(&(phase->t_mapf), l_in->p_path))
			{
				growstring temp;
				temp = without_bslash(getvar(LIBVAR));
				ptr = gr_final(&temp);
#ifdef DEBUG
				if (debug >= 4)
				{
					vprint("phase %s, library %s(%s)\n", phase->t_name, l_in->p_path, ptr);
				}
#endif
				if (l_in->p_keeps)
					throws(l_in->p_path);
				l_in->p_path = ptr;
				l_in->p_keeps = YES;
			}
		}
		scanlist(l_first(flags), elem)
		{
			/* Get the flags remaining for the loader,
			   That is: all the flags neither eaten by ack nor
			   one of the subprograms called so-far.
			   The last fact is indicated by the NO_SCAN bit
			   in the first character of the flag.
			*/
			if (!(*(l_content(*elem)) & NO_SCAN))
			{
				l_add(&(phase->t_flags), l_content(*elem));
#ifdef DEBUG
				if (debug >= 4)
				{
					vprint("phase %s, added flag %s\n", phase->t_name, l_content(*elem));
				}
#endif
			}
		}
	}
}

static char* headvar(void)
{
	if (!touch_head)
		return "";
	return gr_start(head);
}

void add_head(const char* str)
{
	if (!touch_head)
	{
		gr_init(&head);
		touch_head = YES;
	}
	gr_cat(&head, str);
}

static char* tailvar(void)
{
	if (!touch_tail)
		return "";
	return gr_start(tail);
}

void add_tail(const char* str)
{
	if (!touch_tail)
	{
		gr_init(&tail);
		touch_tail = YES;
	}
	gr_cat(&tail, str);
}

void transini(void)
{
	register list_elem* elem;
	register trf* phase;

	scanlist(l_first(tr_list), elem)
	{
		phase = t_cont(*elem);
		if (!phase->t_linker)
			getmapflags(phase);
	}
	scanlist(l_first(R_list), elem)
	{
		set_Rflag(l_content(*elem));
	}
	l_clear(&R_list);
	setpvar(keeps(HEAD), headvar);
	setpvar(keeps(TAIL), tailvar);
}

static void set_Rflag(char* argp)
{
	register char* eos;
	register list_elem* prog;
	register int length;
	char *eq, *colon;

	eos = strchr(&argp[2], '-');
	eq = strchr(&argp[2], EQUAL);
	colon = strchr(&argp[2], ':');
	if (!eos)
	{
		eos = eq;
	}
	else
	{
		if (eq && eq < eos)
			eos = eq;
	}
	if (colon && (!eos || eos > colon))
		eos = colon;
	if (!eos)
	{
		if (!(argp[0] & NO_SCAN))
			werror("Incorrect use of -R flag");
		return;
	}
	length = eos - &argp[2];
	scanlist(l_first(tr_list), prog)
	{
		if (strncmp(t_cont(*prog)->t_name, &argp[2], length) == 0
		    && t_cont(*prog)->t_name[length] == 0 /* Same name length */)
		{
			if (*eos == '-')
			{
				if (!(argp[0] & NO_SCAN))
				{
					/* If not already taken by a mapflag */
					l_add(&(t_cont(*prog)->t_flags), eos);
				}
			}
			else if (*eos == '=')
			{
				t_cont(*prog)->t_prog = keeps(eos + 1);
			}
			else
			{
				t_cont(*prog)->t_priority = atoi(eos + 1);
			}
			argp[0] |= NO_SCAN;
			return;
		}
	}
	if (!(argp[0] & NO_SCAN))
		werror("Cannot find program for %s", argp);
	return;
}

/**************************************************************************/
/*                                                                        */
/*           The creation of arguments for exec for a transformation      */
/*                                                                        */
/**************************************************************************/

static growstring scanvars(const char* line)
{
	/* Scan a line variable replacements started by S_VAR.
	   Two sequences exist: S_VAR name C_VAR, S_VAR name A_VAR text C_VAR.
	   neither name nor text may contain further replacements.
	   In the first form an error message is issued if the name is not
	   present in the variables, the second form produces text
	   in that case.
	*/
	const char* in_c;
	int token, token_r;
	growstring result, name;
	char* tr;
	enum
	{
		TEXT,
		FIRST,
		NAME,
		SKIP,
		COPY
	} state
	    = TEXT;
	int escaped = NO;

	gr_init(&result);
	gr_init(&name);
	for (in_c = line; *in_c; in_c++)
	{
		token = *in_c & 0377;
		token_r = (escaped ? 0 : token);

		/* A backslash escapes the next character. */
		if (token_r == BSLASH)
		{
			if (state == TEXT || state == COPY)
			{
				/* Keep BSLASH for later scans. */
				gr_add(&result, token);
			}
			escaped = YES;
			continue;
		}
		escaped = NO;

		switch (state)
		{
			case TEXT:
				if (token_r == S_VAR)
				{
					state = FIRST;
				}
				else
				{
					gr_add(&result, token);
				}
				break;
			case FIRST:
				switch (token_r)
				{
					case A_VAR:
					case C_VAR:
						fatal("empty string variable name");
					default:
						state = NAME;
						gr_add(&name, token);
						break;
				}
				break;
			case NAME:
				switch (token_r)
				{
					case A_VAR:
						gr_add(&name, 0);
						tr = getvar(gr_start(name));
						if (tr != NULL)
						{
							while (*tr)
							{
								gr_add(&result, *tr++);
							}
							state = SKIP;
						}
						else
						{
							state = COPY;
						}
						gr_throw(&name);
						break;
					case C_VAR:
						gr_add(&name, 0);
						tr = getvar(gr_start(name));
						if (tr != NULL)
						{
							while (*tr)
							{
								gr_add(&result, *tr++);
							}
						}
						else
						{
							werror("No definition for %s", gr_start(name));
						}
						state = TEXT;
						gr_throw(&name);
						break;
					default:
						gr_add(&name, token);
						break;
				}
				break;
			case SKIP:
				if (token_r == C_VAR)
					state = TEXT;
				break;
			case COPY:
				if (token_r == C_VAR)
					state = TEXT;
				else
				{
					gr_add(&result, token);
				}
				break;
		}
	}
	gr_add(&result, 0);
	if (escaped)
		werror("flag line ends with %c", BSLASH);
	if (state != TEXT)
	{
		werror("flag line misses %c", C_VAR);
		gr_throw(&name);
	}
	return result;
}

static growstring scanexpr(const char* line)
{
	/* Scan a line for conditional or flag expressions,
	   dependent on the type. The format is
	   S_EXPR suflist M_EXPR suflist T_EXPR tail C_EXPR
	   the head and tail are passed to condit(), together with the
	   growstring for futher treatment.
	   Nesting is not allowed.
	*/
	const char *in_c, *heads;
	int token, token_r;
	growstring sufs, tailval, result;
	static list_head fsuff, lsuff;
	enum
	{
		TEXT,
		FDOT,
		FSUF,
		LDOT,
		LSUF,
		FTAIL
	} state
	    = TEXT;
	int escaped = NO;

	gr_init(&result);
	gr_init(&sufs);
	gr_init(&tailval);
	for (in_c = line; *in_c; in_c++)
	{
		token = *in_c & 0377;
		token_r = (escaped ? 0 : token);

		/* A backslash escapes the next character. */
		if (token_r == BSLASH)
		{
			if (state == TEXT || state == FTAIL)
			{
				/* Keep BSLASH for later scans. */
				gr_add(&result, token);
			}
			escaped = YES;
			continue;
		}
		escaped = NO;

		switch (state)
		{
			case TEXT:
				if (token_r == S_EXPR)
				{
					state = FDOT;
					heads = in_c;
				}
				else
					gr_add(&result, token);
				break;
			case FDOT:
				if (token_r == M_EXPR)
				{
					state = LDOT;
					break;
				}
				if (token != SUFCHAR)
				{
					error("Missing %c in expression", SUFCHAR);
				}
				gr_add(&sufs, token);
				state = FSUF;
				break;
			case FSUF:
				if (token_r == M_EXPR || token == SUFCHAR)
				{
					gr_add(&sufs, 0);
					l_add(&fsuff, gr_final(&sufs));
				}
				if (token_r == M_EXPR)
				{
					state = LDOT;
				}
				else
					gr_add(&sufs, token);
				break;
			case LDOT:
				if (token_r == T_EXPR)
				{
					state = FTAIL;
					break;
				}
				if (token != SUFCHAR)
				{
					error("Missing %c in expression", SUFCHAR);
				}
				gr_add(&sufs, token);
				state = LSUF;
				break;
			case LSUF:
				if (token_r == T_EXPR || token == SUFCHAR)
				{
					gr_add(&sufs, 0);
					l_add(&lsuff, gr_final(&sufs));
				}
				if (token_r == T_EXPR)
				{
					state = FTAIL;
				}
				else
					gr_add(&sufs, token);
				break;
			case FTAIL:
				if (token_r == C_EXPR)
				{
					/* Found one !! */
					gr_add(&tailval, 0);
					condit(&result, &fsuff, &lsuff, gr_start(tailval));
					l_throw(&fsuff);
					l_throw(&lsuff);
					gr_throw(&tailval);
					state = TEXT;
				}
				else
					gr_add(&tailval, token);
				break;
		}
	}
	gr_add(&result, 0);
	if (state != TEXT)
	{
		l_throw(&fsuff);
		l_throw(&lsuff);
		gr_throw(&tailval);
		werror("flag line has unclosed expression starting with %6s", heads);
	}
	return result;
}

static void condit(growstring* line, list_head* fsuff, list_head* lsuff, char* tailval)
{
	register list_elem* first;
	register list_elem* last;

#ifdef DEBUG
	if (debug >= 4)
		vprint("Conditional for %s, ", tailval);
#endif
	scanlist(l_first(*fsuff), first)
	{
		scanlist(l_first(*lsuff), last)
		{
			if (strcmp(l_content(*first), l_content(*last)) == 0)
			{
				/* Found */
#ifdef DEBUG
				if (debug >= 4)
					vprint(" matched\n");
#endif
				while (*tailval)
					gr_add(line, *tailval++);
				return;
			}
		}
	}
#ifdef DEBUG
	if (debug >= 4)
		vprint(" non-matched\n");
#endif
}

static int mapflag(list_head* maplist, const char* cflag)
{
	/* Expand a flag expression */
	/* The flag "cflag" is checked for each of the mapflags.
	   A mapflag entry has the form
	    -text NAME=replacement or -text*text NAME=replacement
	   The star matches anything as in the shell.
	   If the entry matches the assignment will take place
	   This replacement is subjected to argument matching only.
	   When a match took place the replacement is returned
	   when not, (char *)0.
	   The replacement sits in stable storage.
	*/
	register list_elem* elem;

	scanlist(l_first(*maplist), elem)
	{
		if (mapexpand(l_content(*elem), cflag))
		{
			return 1;
		}
	}
	return 0;
}

static int mapexpand(char* mapentry, const char* cflag)
{
	const char* star;
	char *ptr, *space;
	int length;

	star = strchr(mapentry, STAR);
	space = firstblank(mapentry);
	if (star > space)
		star = (char*)0;
	if (star)
	{
		length = space - star - 1;
		if (strncmp(mapentry, cflag, star - mapentry)
		    || strncmp(star + 1, cflag + strlen(cflag) - length, length))
		{
			return 0;
		}
		/* Match */
		/* Now set star to the first char of the star
		   replacement and length to its length
		*/
		length = strlen(cflag) - (star - mapentry) - length;
		if (length < 0)
			return 0;
		star = cflag + (star - mapentry);
#ifdef DEBUG
		if (debug >= 6)
		{
			vprint("Starmatch (%s,%s) %.*s\n", mapentry, cflag, length, star);
		}
#endif
	}
	else
	{
		if (strncmp(mapentry, cflag, space - mapentry) != 0 || cflag[space - mapentry])
		{
			return 0;
		}
	}
	ptr = skipblank(space);
	if (*ptr == 0)
		return 1;
	doassign(ptr, star, length);
	return 1;
}

void doassign(const char* line, const char* star, int length)
{
	growstring varval, name, temp;
	const char* ptr;
	int escaped = NO;

	gr_init(&varval);
	gr_init(&name);
	ptr = line;
	for (; *ptr && *ptr != SPACE && *ptr != TAB && *ptr != EQUAL; ptr++)
	{
		gr_add(&name, *ptr);
	}
	ptr = strchr(ptr, EQUAL);
	if (!ptr)
	{
		error("Missing %c in assignment %s", EQUAL, line);
		return;
	}
	temp = scanvars(ptr + 1);
	for (ptr = gr_start(temp); *ptr; ptr++)
		switch (*ptr)
		{
			case BSLASH:
				escaped = YES;
				gr_add(&varval, *ptr);
				break;
			case STAR:
				if (star && !escaped)
				{
					while (length--)
					{
						gr_add(&varval, BSLASH);
						gr_add(&varval, *star++);
					}
					break;
				}
				/* FALLTHROUGH */
			default:
				escaped = NO;
				gr_add(&varval, *ptr);
				break;
		}
	gr_throw(&temp);
	gr_add(&name, 0);
	gr_add(&varval, 0);
	setsvar(gr_final(&name), gr_final(&varval));
}

#define ISBLANK(c) ((c) == SPACE || (c) == TAB)

static void unravel(const char* line, void (*action)(char*))
{
	/* Unravel the line, get arguments a la shell */
	/* each argument is handled to action */
	/* The input string is left intact */
	const char* in_c;
	int token;
	enum
	{
		BLANK,
		ARG,
		ESCAPED
	} state
	    = BLANK;
	growstring argum;

	/* Loop for each character of line, including final '\0' */
	in_c = line;
	for (;;)
	{
		token = *in_c & 0377;
		switch (state)
		{
			case BLANK:
				if (token == 0)
					break;
				if (!ISBLANK(token))
				{
					gr_init(&argum);
					gr_add(&argum, token);
					if (token == BSLASH)
					{
						state = ESCAPED;
					}
					else
					{
						state = ARG;
					}
				}
				break;
			case ARG:
				if (ISBLANK(token) || token == 0)
				{
					gr_add(&argum, 0);
					(*action)(gr_start(argum));
					gr_throw(&argum);
					state = BLANK;
				}
				else
				{
					gr_add(&argum, token);
					if (token == BSLASH)
						state = ESCAPED;
				}
				break;
			case ESCAPED:
				gr_add(&argum, token);
				state = ARG;
				break;
		}
		if (token == 0)
			break;
		in_c++;
	}
}

static list_head* curargs;
static list_head* comb_args;

static void addargs3(const char* prefix1, const char* prefix2, const char* string)
{
	/* Prepend prefix1 and prefix2 to string, then add it to
	   curargs.  string is scanned to strip backslashes and
	   substitute files for C_IN and C_OUT.  prefixes are not
	   scanned.
	*/
	const char* in_c;
	int token;
	char* tr;
	list_elem* elem;
	growstring argum;
	int escaped = NO;

	gr_init(&argum);
	for (in_c = prefix1; *in_c; in_c++)
	{
		gr_add(&argum, *in_c);
	}
	for (in_c = prefix2; *in_c; in_c++)
	{
		gr_add(&argum, *in_c);
	}
	for (in_c = string; *in_c; in_c++)
	{
		token = *in_c & 0377;
		if (escaped)
		{
			/* Strip BSLASH, keep escaped character. */
			gr_add(&argum, token);
			escaped = NO;
			continue;
		}
		switch (token)
		{
			case BSLASH:
				escaped = YES;
				break;
			case C_IN: /* Input file */
				if (in.p_path)
				{ /* Not for the combiners */
					for (tr = in.p_path; *tr; tr++)
					{
						gr_add(&argum, *tr);
					}
				}
				else
				{ /* For the combiners */
					gr_add(&argum, 0);
					tr = gr_final(&argum);
					in_c++;
					scanlist(l_first(*comb_args), elem)
					{
						char* p = p_cont(*elem)->p_path;
						addargs3(tr, p, in_c);
					}
					throws(tr);
					return;
				}
				break;
			case C_OUT: /* Output file */
#ifdef DEBUG
				if (!out.p_path)
					fatal("missing output filename");
#endif
				for (tr = out.p_path; *tr; tr++)
				{
					gr_add(&argum, *tr);
				}
				break;
			default:
				gr_add(&argum, token);
				break;
		}
	}
	gr_add(&argum, 0);
	tr = gr_final(&argum);
	l_add(curargs, tr);
}

static void addargs(char* string)
{
	addargs3("", "", string);
}

static void getcallargs(trf* phase)
{
	growstring arg1, arg2;

	arg1 = scanvars(phase->t_argd);
#ifdef DEBUG
	if (debug >= 3)
		vprint("\tvars: %s", gr_start(arg1));
#endif
	arg2 = scanexpr(gr_start(arg1));
#ifdef DEBUG
	if (debug >= 3)
		vprint("\texpr: %s", gr_start(arg2));
#endif
	gr_throw(&arg1);
	curargs = &phase->t_args;
	if (phase->t_combine)
		comb_args = &phase->t_inputs;
	unravel(gr_start(arg2), addargs);
	gr_throw(&arg2);
}

static growstring without_bslash(const char* string)
{
	/* Strip backslashes from a copy of the string. */
	growstring result;
	const char* in_c;
	int token;
	int escaped = NO;

	gr_init(&result);
	for (in_c = string; *in_c; in_c++)
	{
		token = *in_c & 0377;
		if (token == BSLASH && !escaped)
		{
			escaped = YES;
		}
		else
		{
			gr_add(&result, token);
			escaped = NO;
		}
	}
	gr_add(&result, 0);
	return result;
}

static void getprogram(trf* phase)
{
	growstring prog1, prog2;
	const char* in_c;
	int token;
	int escaped = NO;

	/* Expand string variables in program name. */
	prog1 = scanvars(phase->t_prog);
	throws(phase->t_prog);

	/* Strip backslashes. */
	prog2 = without_bslash(gr_start(prog1));
	gr_throw(&prog1);
	phase->t_prog = gr_final(&prog2);
}
