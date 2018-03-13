/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <em_mnem.h>
#include "../share/types.h"
#include "../share/aux.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "cs.h"
#include "cs_aux.h"
#include "cs_entity.h"
#include "cs_stack.h"

#define WS1	0
#define WS2	1
#define PS	2
#define ARGW	3
#define ARDESC3	4

STATIC struct inf_entity {
	byte	inf_instr;	/* Key.				*/
	byte	inf_used;	/* Kind of entity used by key.	*/
	byte	inf_size;	/* Indication of the size.	*/
} inf_table[] = {
	op_adp,	ENAOFFSETTED,	PS,
	op_dee,	ENEXTERNAL,	WS1,
	op_del,	ENLOCAL,	WS1,
	op_ine,	ENEXTERNAL,	WS1,
	op_inl,	ENLOCAL,	WS1,
	op_lae,	ENAEXTERNAL,	PS,
	op_lal,	ENALOCAL,	PS,
	op_lar,	ENARRELEM,	ARDESC3,
	op_ldc,	ENCONST,	WS2,
	op_lde,	ENEXTERNAL,	WS2,
	op_ldf,	ENOFFSETTED,	WS2,
	op_ldl,	ENLOCAL,	WS2,
	op_lil,	ENINDIR,	WS1,
	op_lim,	ENIGNMASK,	WS1,
	op_loc,	ENCONST,	WS1,
	op_loe,	ENEXTERNAL,	WS1,
	op_lof,	ENOFFSETTED,	WS1,
	op_loi,	ENINDIR,	ARGW,
	op_lol,	ENLOCAL,	WS1,
	op_lpi,	ENPROC,		PS,
	op_lxa,	ENAARGBASE,	PS,
	op_lxl,	ENALOCBASE,	PS,
	op_sar,	ENARRELEM,	ARDESC3,
	op_sde,	ENEXTERNAL,	WS2,
	op_sdf,	ENOFFSETTED,	WS2,
	op_sdl,	ENLOCAL,	WS2,
	op_sil,	ENINDIR,	WS1,
	op_sim,	ENIGNMASK,	WS1,
	op_ste,	ENEXTERNAL, 	WS1,
	op_stf,	ENOFFSETTED,	WS1,
	op_sti,	ENINDIR,	ARGW,
	op_stl,	ENLOCAL,	WS1,
	op_zer,	ENCONST,	ARGW,
	op_zre,	ENEXTERNAL,	WS1,
	op_zrf,	ENFZER,		ARGW,
	op_zrl,	ENLOCAL,	WS1,
	op_nop	/* Delimitor. */
};

#define INFKEY(ip)	(ip->inf_instr & BMASK)
#define ENKIND(ip)	ip->inf_used
#define SIZEINF(ip)	ip->inf_size

STATIC struct inf_entity *getinf(int n)
{
	struct inf_entity *ip;

	for (ip = &inf_table[0]; INFKEY(ip) != op_nop; ip++) {
		if (INFKEY(ip) == n) return ip;
	}
	return (struct inf_entity *) 0;
}

entity_p getentity(line_p lnp, line_p *l_out)
{
	/* Build the entities where lnp refers to, and enter them.
	 * If a token needs to be popped, the first line that pushed
	 * it is stored in *l_out.
	 * The main entity lnp refers to, is returned.
	 */
	struct entity en;
	struct token tk;
	struct inf_entity *ip;
	valnum vn;
	offset indexsize;
	struct token adesc, index, arbase;

	*l_out = lnp;

	/* Lor is a special case. */
	if (INSTR(lnp) == op_lor) {
		offset off = off_set(lnp);

		en.en_static = FALSE;
		en.en_size = ps;
		switch ((int) off == off ? (int) off : 3) {
			default:
				assert(FALSE);
				break;
			case 0:
				en.en_kind = ENLOCBASE;
				break;
			case 1:
				return (entity_p) 0;
			case 2:
				en.en_kind = ENHEAPPTR;
				break;
		}
		return en_enter(&en);
	}

	if ( (ip = getinf(INSTR(lnp))) == (struct inf_entity *) 0)
		return (entity_p) 0; /* It does not refer to any entity. */

	/* Lil and sil refer to two entities. */
	if (INSTR(lnp) == op_lil || INSTR(lnp) == op_sil) {
		en.en_static = FALSE;
		en.en_kind = ENLOCAL;
		en.en_size = ps; /* Local must be a pointer. */
		en.en_loc = off_set(lnp);
		vn = en_enter(&en)->en_vn;
	}

	en.en_static = FALSE;
	en.en_kind = ENKIND(ip);

	/* Fill in the size of the entity. */
	switch (SIZEINF(ip)) {
		default:
			assert(FALSE);
			break;
		case WS1:
			en.en_size = ws;
			break;
		case WS2:
			en.en_size = 2*ws;
			break;
		case PS:
			en.en_size = ps;
			break;
		case ARGW:
			if (TYPE(lnp) != OPNO) {
				en.en_size = off_set(lnp);
			} else {
				Pop(&tk, (offset) ws);
				*l_out = tk.tk_lfirst;
				en.en_size = UNKNOWN_SIZE;
			}
			break;
		case ARDESC3:
			assert(en.en_kind == ENARRELEM);
			if (TYPE(lnp) != OPNO) {
				indexsize = off_set(lnp);
			} else {
				Pop(&tk, (offset) ws);
				indexsize = UNKNOWN_SIZE;
			}
			Pop(&adesc, (offset) ps);
			en.en_adesc = adesc.tk_vn;
			Pop(&index, indexsize);
			en.en_index = index.tk_vn;
			Pop(&arbase, (offset) ps);
			en.en_arbase = arbase.tk_vn;
			*l_out = arbase.tk_lfirst;
			en.en_size = array_elemsize(adesc.tk_vn);
			break;
	}

	/* Fill in additional information. */
	switch (en.en_kind) {
		case ENFZER:
			en.en_static = TRUE;
			break;
		case ENCONST:
			en.en_static = TRUE;
			en.en_val = off_set(lnp);
			break;
		case ENALOCAL:
			en.en_static = TRUE;
		case ENLOCAL:
			en.en_loc = off_set(lnp);
			break;
		case ENAEXTERNAL:
			en.en_static = TRUE;
		case ENEXTERNAL:
			en.en_ext = OBJ(lnp);
			break;
		case ENINDIR:
			if (INSTR(lnp) == op_loi || INSTR(lnp) == op_sti) {
				Pop(&tk, (offset) ps);
				*l_out = tk.tk_lfirst;
				vn = tk.tk_vn;
			}
			en.en_ind = vn;
			break;
		case ENAOFFSETTED:
			en.en_static = TRUE;
		case ENOFFSETTED:
			Pop(&tk, (offset) ps);
			*l_out = tk.tk_lfirst;
			en.en_base = tk.tk_vn;
			en.en_off = off_set(lnp);
			break;
		case ENALOCBASE:
		case ENAARGBASE:
			en.en_levels = off_set(lnp);
			if (en.en_levels == 0) {
				/* otherwise the program could change it */
				en.en_static = TRUE;
			}
			break;
		case ENPROC:
			en.en_pro = PROC(lnp);
			break;
		case ENARRELEM:
			/* We gathered the information in the previous switch.
			 */
			break;
	}

	return en_enter(&en);
}
