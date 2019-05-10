/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"


int stackheight = 0;
token_t fakestack[MAXFSTACK];
int nallreg = 0;
int allreg[MAXALLREG];
token_p curtoken = (token_p) 0;
result_t dollar[LONGESTPATTERN];
int nemlines =0;
struct emline emlines[MAXEMLINES];
struct emline *emp=emlines;
struct emline *saveemp;
int tokpatlen;
rl_p curreglist;
