#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"

/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 * Author: Hans van Staveren
 */

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
