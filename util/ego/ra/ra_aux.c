/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  A U X I L I A R Y   R O U T I N E S
 */

#include <em_mnem.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_reg.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/alloc.h"
#include "ra.h"
#include "ra_aux.h"


time_p cons_time(l,b)
	line_p l;
	bblock_p b;
{
	/* Construct a time */

	time_p t = newtime();

	t->t_line = l;
	t->t_bblock = b;
	return t;
}




short loop_scale(short lev)
{
	return (lev == 0 ? 1 : (lev > 3 ? 32 : 8 * lev));
}
