/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include <em_spec.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/lset.h"
#include "cs.h"
#include "cs_aux.h"
#include "cs_avail.h"
#include "cs_debug.h"
#include "cs_entity.h"

#ifdef VERBOSE

extern char em_mnem[]; /* The mnemonics of the EM instructions. */

STATIC void showinstr(line_p lnp)
{
	/* Makes the instruction in `lnp' human readable. Only lines that
	 * can occur in expressions that are going to be eliminated are
	 * properly handled.
	 */
	if (INSTR(lnp) < sp_fmnem && INSTR(lnp) > sp_lmnem) {
		fprintf(stderr,"*** ?\n");
		return;
	}

	fprintf(stderr,"%s", &em_mnem[4 * (INSTR(lnp)-sp_fmnem)]);
	switch (TYPE(lnp)) {
		case OPNO:
			break;
		case OPSHORT:
			fprintf(stderr," %d", SHORT(lnp));
			break;
		case OPOBJECT:
			fprintf(stderr," %d", OBJ(lnp)->o_id);
			break;
		case OPOFFSET:
			fprintf(stderr," %ld", OFFSET(lnp));
			break;
		default:
			fprintf(stderr," ?");
			break;
	}
	fprintf(stderr,"\n");
}

void SHOWOCCUR(occur_p ocp)
{
	/* Shows all instructions in an occurrence. */

	register line_p lnp, next;

	if (verbose_flag) {
		for (lnp = ocp->oc_lfirst; lnp != (line_p) 0; lnp = next) {
			next = lnp == ocp->oc_llast ? (line_p) 0 : lnp->l_next;

			showinstr(lnp);
		}
	}
}

#endif

#ifdef TRACE

void SHOWAVAIL(avail_p avp)
{
	/* Shows an available expression. */
	showinstr(avp->av_found);
	fprintf(stderr,"result %d,", avp->av_result);
	fprintf(stderr,"occurred %d times\n", Lnrelems(avp->av_occurs) + 1);

}

void OUTAVAILS(void)
{
	register avail_p ravp;

	fprintf(stderr,"AVAILABLE EXPRESSIONS\n");

	for (ravp = avails; ravp != (avail_p) 0; ravp = ravp->av_before) {
		SHOWAVAIL(ravp);
		fprintf(stderr,"\n");
	}
}

STATIC char *enkinds[] = {
	"constant",
	"local",
	"external",
	"indirect",
	"offsetted",
	"address of local",
	"address of external",
	"address of offsetted",
	"address of local base",
	"address of argument base",
	"procedure",
	"floating zero",
	"array element",
	"local base",
	"heap pointer",
	"ignore mask"
};

void OUTENTITIES(void)
{
	register Lindex i;

	fprintf(stderr,"ENTITIES\n");
	for (i = Lfirst(entities); i != (Lindex) 0; i = Lnext(i, entities)) {
		register entity_p rep = en_elem(i);

		fprintf(stderr,"%s,", enkinds[rep->en_kind]);
		fprintf(stderr,"size %ld,", rep->en_size);
		fprintf(stderr,"valno %d,", rep->en_vn);
		switch (rep->en_kind) {
			case ENCONST:
				fprintf(stderr,"$%ld\n", rep->en_val);
				break;
			case ENLOCAL:
			case ENALOCAL:
				fprintf(stderr,"%ld(LB)\n", rep->en_loc);
				break;
			case ENINDIR:
				fprintf(stderr,"*%d\n", rep->en_ind);
				break;
			case ENOFFSETTED:
			case ENAOFFSETTED:
				fprintf(stderr,"%ld(%d)\n", rep->en_off, rep->en_base);
				break;
			case ENALOCBASE:
			case ENAARGBASE:
				fprintf(stderr,"%ld levels\n", rep->en_levels);
				break;
			case ENARRELEM:
				fprintf(stderr,"%d[%d], ",rep->en_arbase,rep->en_index);
				fprintf(stderr,"rom at %d\n", rep->en_adesc);
				break;
		}
		fprintf(stderr,"\n");
	}
}

#endif /* TRACE */
