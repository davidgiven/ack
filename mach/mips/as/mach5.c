/*
 * MIPS assembler helpers.
 */

extern item_t *last_it;

int
mips_small_relo(expr_t *e, int relotype, valu_t *out)
{
	valu_t addend;
	unsigned short nami;

	if ((e->val & 0xffff0000) == 0) {
		newrelo(e->typ, relotype | FIXUPFLAGS);
		*out = e->val;
		return 1;
	}

	if (pass == PASS_1) {
		*out = 0;
		return 1;
	}

#ifndef ASLD
	nami = item_get_nami(last_it);
	if (last_it != 0 && nami != 0
	    && (last_it->i_type & S_TYP) >= S_MIN
	    && (last_it->i_type & S_TYP) != S_UND) {
		addend = e->val - last_it->i_valu;
		if ((addend & 0xffff0000) == 0) {
			valu_t saved = relonami;

			relonami = nami;
			newrelo(S_UND, relotype | FIXUPFLAGS);
			relonami = saved;
			*out = addend;
			return 1;
		}
	}
#endif

	return 0;
}
