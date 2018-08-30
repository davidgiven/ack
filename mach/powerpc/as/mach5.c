static int hl_token;
static expr_t hl_expr;

void no_hl(void) {
	hl_token = 0;
}

word_t eval_hl(expr_t* expr, int token)
{
	word_t val = expr->val;
	uint16_t hi = val >> 16;
	uint16_t lo = val & 0xffff;

	hl_token = token;
	hl_expr = *expr;

	switch (token) {
	case OP_HI:  /* hi16[expr] */
		return hi;
	case OP_HA:  /* ha16[expr]*/
		/*
		 * If the low half will be treated as a signed value,
		 * then values greater than 0x7fff will cause the high
		 * half to have 1 subtracted from it; so we apply an
		 * adjustment here.
		 */
		if (lo > 0x7fff)
			hi++;
		return hi;
	case OP_LO:  /* lo16[expr] */
		return lo;
	}
}

void emit_hl(word_t in)
{
	word_t reg;
	int type;

	switch (hl_token) {
	case OP_HI:  /* hi16[expr] */
	case OP_HA:  /* ha16[expr] */
		if (PASS_RELO && (hl_expr.typ & S_TYP) != S_ABS) {
			/*
			 * RELOPPC_LIS only works with lis _, _ (same
			 * as addis _, r0, _).  Check if instruction
			 * isn't addis or register RA isn't r0.
			 */
			if ((in & 0xfc1f0000) != (0x3c000000))
				serror("relocation only works with lis");

			/*
			 * High bit: ha16 flag
			 * Next 5 bits: register RT
			 * Low 26 bits: signed offset
			 */
			fit(fitx(hl_expr.val, 26));
			newrelo(hl_expr.typ, RELOPPC_LIS | FIXUPFLAGS);
			reg = (in >> 21) & 0x1f;
			in = (hl_token == OP_HA) << 31;
			in |= reg << 26;
			in |= hl_expr.val & 0x03ffffff;
		}
		break;
	case OP_LO:  /* lo16[expr] */
		if (PASS_RELO && (hl_expr.typ & S_TYP) != S_ABS) {
			DOTVAL += 2;
			newrelo(hl_expr.typ, RELO2 | FIXUPFLAGS);
			DOTVAL -= 2;
		}
		break;
	}

	emit4(in);
}
