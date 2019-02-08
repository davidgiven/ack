/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ L I F E T I M E . C
 */

#include <em_mnem.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_reg.h>
#include <em_mes.h>
#include <em_ego.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/utils.h"
#include "../share/alloc.h"
#include "ra.h"
#include "ra_aux.h"
#include "ra_items.h"
#include "ra_lifet.h"


#define MSG_OFF(l)	aoff(ARG(l),2)
#define is_livemsg(l)	(INSTR(l) == ps_mes && aoff(ARG(l),0) == ms_ego && \
			 aoff(ARG(l),1) == ego_live)
#define is_deadmsg(l)	(INSTR(l) == ps_mes && aoff(ARG(l),0) == ms_ego && \
			 aoff(ARG(l),1) == ego_dead)

build_lifetimes(items)
	item_p items[];
{
	/* compute the it_lives attribute of every item; this is
	 * a list of intervals during which the item is live,
	 * i.e. its current value may be used.
	 * We traverse the EM text of the current procedure in
	 * lexical order. If we encounter a live-message, we store
	 * the number ('time') of the current instruction in the
	 * it_lastlive attribute of the concerning item. If we see
	 * a dead-message for that item, we know that the item is
	 * live in between these two pseudo's. If the first message
	 * appearing in the procedure is a dead-message, the item
	 * is live from time 0 (start of procedure) till now. (Note
	 * that it_lastlive is initially 0!).
	 * The lifetime ends on the last instruction before the
	 * dead-message that is not a live -or dead message.
	 */

	register line_p l;
	register short now;
	item_p item;
	short last_code;

	last_code = 0;
	for (now = 0; now < nrinstrs; now++) {
		l = instrmap[now];
		if (is_livemsg(l)) {
			item = item_of(MSG_OFF(l),items);
			/* A local variable that is never used is NOT an
			 * item; yet, there may be a register message for it...
			 */
			if(item != (item_p) 0) {
				item->it_lastlive = last_code + 1;
			}
		} else {
			if (is_deadmsg(l)) {
				item = item_of(MSG_OFF(l),items);
				if (item != (item_p) 0) {
					add_interval(item->it_lastlive,
					       last_code, &item->it_lives);
				}
			} else {
				last_code = now;
			}
		}
	}
}
