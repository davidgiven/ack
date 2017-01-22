/*
 * $Source$
 * $State$
 */

operation
	: OP                                              { emit4($1); }
	| OP_BDA               bda                        { emit4($1 | $2); }
	| OP_BDL               bdl                        { emit4($1 | $2); }
	| OP_BF_BFA            CR ',' CR                  { emit4($1 | ($2<<23) | ($4<<18)); }
	| OP_BF_FRA_FRB        CR ',' FPR ',' FPR         { emit4($1 | ($2<<23) | ($4<<16) | ($6<<11)); }
	| OP_BF_L_RA_RB        CR ',' u1 ',' GPR ',' GPR  { emit4($1 | ($2<<23) | ($4<<21) | ($6<<16) | ($8<<11)); }
	| OP_BF_L_RA_SI        CR ',' u1 ',' GPR ',' e16  { emit4($1 | ($2<<23) | ($4<<21) | ($6<<16) | $8); }
	| OP_BF_L_RA_UI        CR ',' u1 ',' GPR ',' e16  { emit4($1 | ($2<<23) | ($4<<21) | ($6<<16) | $8); }
	| OP_BF_RA_RB          cr_opt GPR ',' GPR         { emit4($1 | ($2<<23) | ($3<<16) | ($5<<11)); }
	| OP_BF_RA_SI          cr_opt GPR ',' e16         { emit4($1 | ($2<<23) | ($3<<16) | $5); }
	| OP_BF_RA_UI          cr_opt GPR ',' e16         { emit4($1 | ($2<<23) | ($3<<16) | $5); }
	| OP_BF_U_C            c CR ',' u4                { emit4($1 | $2 | ($3<<23) | ($5<<12)); }
	| OP_BH                                           { emit4($1); }
	| OP_BH                u2                         { emit4($1 | ($2<<11)); }
	| OP_BI_BDA            u5 ',' bda                 { emit4($1 | ($2<<16) | $4); }
	| OP_BI_BDL            u5 ',' bdl                 { emit4($1 | ($2<<16) | $4); }
	| OP_BI_BH             u5 opt_bh                  { emit4($1 | ($2<<16) | $3); }
	| OP_BICR_BDA          cr_opt bda                 { emit4($1 | ($2<<18) | $3); }
	| OP_BICR_BDL          cr_opt bdl                 { emit4($1 | ($2<<18) | $3); }
	| OP_BICR_BH                                      { emit4($1); }
	| OP_BICR_BH           CR opt_bh                  { emit4($1 | ($2<<18) | $3); }
	| OP_BO_BI_BDA         u5 ',' u5 ',' bda          { emit4($1 | ($2<<21) | ($4<<16) | $6); }
	| OP_BO_BI_BDL         u5 ',' u5 ',' bdl          { emit4($1 | ($2<<21) | ($4<<16) | $6); }
	| OP_BO_BI_BH          u5 ',' u5 opt_bh           { emit4($1 | ($2<<21) | ($4<<16) | $5); }
	| OP_BT_BA_BA          u5 ',' u5                  { emit4($1 | ($2<<21) | ($4<<16) | ($4<<11)); }
	| OP_BT_BA_BB          u5 ',' u5 ',' u5           { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_BT_BT_BT          u5                         { emit4($1 | ($2<<21) | ($2<<16) | ($2<<11)); }
	| OP_BT_C              c u5                       { emit4($1 | $2 | ($3<<21)); }
	| OP_FLM_FRB_C         c u8 ',' FPR               { emit4($1 | $2 | ($3<<17) | ($5<<11)); }
	| OP_FRS_RA_D          FPR ',' e16 '(' GPR ')'    { emit4($1 | ($2<<21) | ($6<<16) | $4); }
	| OP_FRS_RA_RB         FPR ',' GPR ',' GPR        { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_FRT_FRA_FRB_C     c FPR ',' FPR ',' FPR      { emit4($1 | $2 | ($3<<21) | ($5<<16) | ($7<<11)); }
	| OP_FRT_FRA_FRC_FRB_C c FPR ',' FPR ',' FPR ',' FPR { emit4($1 | $2 | ($3<<21) | ($5<<16) | ($9<<11) | ($7<<6)); }
	| OP_FRT_FRA_FRC_C     c FPR ',' FPR ',' FPR      { emit4($1 | $2 | ($3<<21) | ($5<<16) | ($7<<6)); }
	| OP_FRT_FRB_C         c FPR ',' FPR              { emit4($1 | $2 | ($3<<21) | ($5<<11)); }
	| OP_FRT_RA_D          FPR ',' e16 '(' GPR ')'    { emit4($1 | ($2<<21) | ($6<<16) | $4); }
	| OP_FRT_RA_RB         FPR ',' GPR ',' GPR        { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_FRT_C             c FPR                      { emit4($1 | $2 | ($3<<21)); }
	| OP_RA_RS_RB_C        c GPR ',' GPR ',' GPR
	{ emit4($1 | $2 | ($5<<21) | ($3<<16) | ($7<<11)); }
	| OP_RA_RS_RB_MB5_ME5_C c GPR ',' GPR ',' GPR ',' u5 ',' u5
	{ emit4($1 | $2 | ($5<<21) | ($3<<16) | ($7<<11) |
		($9<<6) | ($11<<1)); }
	| OP_RA_RS_RB_MB6_C    c GPR ',' GPR ',' GPR ',' u6
	{ emit4($1 | $2 | ($5<<21) | ($3<<16) | ($7<<11) | MB6($9)); }
	| OP_RA_RS_SH5_C       c GPR ',' GPR ',' u5
	{ emit4($1 | $2 | ($5<<21) | ($3<<16) | ($7<<11)); }
	| OP_RA_RS_SH5_MB5_ME5_C c GPR ',' GPR ',' u5 ',' u5 ',' u5
	{ emit4($1 | $2 | ($5<<21) | ($3<<16) |
		($7<<11) | ($9<<6) | ($11<<1)); }
	| OP_RA_RS_SH6_C       c GPR ',' GPR ',' u6
	{ emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6($7)); }
	| OP_RA_RS_SH6_MB6_C   c GPR ',' GPR ',' u6 ',' u6
	{ emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6($7) | MB6($9)); }
	| OP_RT                GPR                        { emit4($1 | ($2<<21)); }
	| OP_RT_RA_C           c GPR ',' GPR              { emit4($1 | $2 | ($3<<21) | ($5<<16)); }
	| OP_RT_RA_D           GPR ',' e16 '(' GPR ')'    { emit4($1 | ($2<<21) | ($6<<16) | $4); }
	| OP_RT_RA_DS          GPR ',' ds '(' GPR ')'     { emit4($1 | ($2<<21) | ($6<<16) | $4); }
	| OP_RT_RA_NB          GPR ',' GPR ',' nb         { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_RT_RA_RB          GPR ',' GPR ',' GPR        { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_RT_RA_RB_C        c GPR ',' GPR ',' GPR      { emit4($1 | $2 | ($3<<21) | ($5<<16) | ($7<<11)); }
	| OP_RT_RA_SI          GPR ',' GPR ',' e16        { emit4($1 | ($2<<21) | ($4<<16) | $6); }
	| OP_RT_RA_SI_addic    c GPR ',' GPR ',' e16      { emit4($1 | ($2<<26) | ($3<<21) | ($5<<16) | $7); }
	| OP_RT_RA_SI_subi     GPR ',' GPR ',' negate16   { emit4($1 | ($2<<21) | ($4<<16) | $6); }
	| OP_RT_RA_SI_subic    c GPR ',' GPR ',' negate16 { emit4($1 | ($2<<26) | ($3<<21) | ($5<<16) | $7); }
	| OP_RT_RB_RA_C        c GPR ',' GPR ',' GPR      { emit4($1 | $2 | ($3<<21) | ($7<<16) | ($5<<11)); }
	| OP_RT_SI             GPR ',' e16                { emit4($1 | ($2<<21) | $4); }
	| OP_RT_SPR            GPR ',' spr_num            { emit4($1 | ($2<<21) | ($4<<11)); }
	| OP_RS_FXM            u7 ',' GPR                 { emit4($1 | ($4<<21) | ($2<<12)); }
	| OP_RS_RA_C           c GPR ',' GPR              { emit4($1 | $2 | ($5<<21) | ($3<<16)); }
	| OP_RS_RA_D           GPR ',' e16 '(' GPR ')'    { emit4($1 | ($2<<21) | ($6<<16) | $4); }
	| OP_RS_RA_DS          GPR ',' ds '(' GPR ')'     { emit4($1 | ($2<<21) | ($6<<16) | $4); }
	| OP_RS_RA_NB          GPR ',' GPR ',' nb         { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_RS_RA_UI          GPR ',' GPR ',' e16        { emit4($1 | ($4<<21) | ($2<<16) | $6); }
	| OP_RS_RA_UI_CC       C GPR ',' GPR ',' e16      { emit4($1 | ($5<<21) | ($3<<16) | $7); }
	| OP_RS_RA_RB          GPR ',' GPR ',' GPR        { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_RS_RA_RB_C        c GPR ',' GPR ',' GPR      { emit4($1 | $2 | ($5<<21) | ($3<<16) | ($7<<11)); }
	| OP_RS_RA_RA_C        c GPR ',' GPR              { emit4($1 | $2 | ($5<<21) | ($3<<16) | ($5<<11)); }
	| OP_RS_SPR            spr_num ',' GPR            { emit4($1 | ($4<<21) | ($2<<11)); }
	| OP_TO_RA_RB          u5 ',' GPR ',' GPR         { emit4($1 | ($2<<21) | ($4<<16) | ($6<<11)); }
	| OP_TO_RA_SI          u5 ',' GPR ',' e16         { emit4($1 | ($2<<21) | ($4<<16) | $6); }
	| OP_TOX_RA_RB         GPR ',' GPR                { emit4($1 | ($2<<16) | ($4<<11)); }
	| OP_TOX_RA_SI         GPR ',' e16                { emit4($1 | ($2<<16) | $4); }
	| OP_LEV                                          { emit4($1); }
	| OP_LEV               u7                         { emit4($1 | ($2<<5)); }
	| OP_LIA               lia                        { emit4($1 | $2); }
	| OP_LIL               lil                        { emit4($1 | $2); }
	| OP_LI32              li32                       /* emitted in subrule */
	| OP_clrlsldi          c GPR ',' GPR ',' u6 ',' u6
	{
		quad mb = ($7 - $9) & 0x3f;
		fit($9 <= $7);
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6($9) | MB6(mb));
	}
	| OP_clrldi            c GPR ',' GPR ',' u6
	{
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6(0) | MB6($7));
	}
	| OP_clrrdi            c GPR ',' GPR ',' u6
	{
		quad me = 63 - $7;
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6(0) | MB6(me));
	}
	| OP_extldi            c GPR ',' GPR ',' u6 ',' u6
	{
		quad me = ($7 - 1) & 0x3f;
		fit($7 > 0);
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6($9) | MB6(me));
	}
	| OP_extrdi            c GPR ',' GPR ',' u6 ',' u6
	{
		quad sh = ($9 + $7) & 0x3f;
		quad mb = (64 - $7) & 0x3f;
		fit($7 > 0);
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6(sh) | MB6(mb));
	}
	| OP_rotrdi            c GPR ',' GPR ',' u6
	{
		quad sh = (64 - $7) & 0x3f;
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6(sh) | MB6(0));
	}
	| OP_sldi              c GPR ',' GPR ',' u6
	{
		quad me = 63 - $7;
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6($7) | MB6(me));
	}
	| OP_srdi              c GPR ',' GPR ',' u6
	{
		quad sh = (64 - $7) & 0x3f;
		emit4($1 | $2 | ($5<<21) | ($3<<16) | SH6(sh) | MB6($7));
	}
	| OP_clrlslwi          c GPR ',' GPR ',' u5 ',' u5
	{
		quad mb = ($7 - $9) & 0x1f;
		quad me = 31 - $9;
		fit($9 <= $7);
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      ($9<<11) | (mb<<6) | (me<<1));
	}
	| OP_clrlwi            c GPR ',' GPR ',' u5
	{
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      (0<<11) | ($7<<6) | (31<<1));
	}
	| OP_clrrwi            c GPR ',' GPR ',' u5
	{
		quad me = 31 - $7;
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      (0<<11) | (0<<6) | (me<<1));
	}
	| OP_extlwi            c GPR ',' GPR ',' u5 ',' u5
	{
		quad me = ($7 - 1) & 0x1f;
		fit($7 > 0);
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      ($9<<11) | (0<<6) | (me<<1));
	}
	| OP_extrwi            c GPR ',' GPR ',' u5 ',' u5
	{
		quad sh = ($9 + $7) & 0x1f;
		quad mb = (32 - $7) & 0x1f;
		fit($7 > 0);
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      (sh<<11) | (mb<<6) | (31<<1));
	}
	| OP_inslwi            c GPR ',' GPR ',' u5 ',' u5
	{
		quad sh = (32 - $9) & 0x1f;
		quad me = ($9 + $7 - 1) & 0x1f;
		fit($7 > 0);
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      (sh<<11) | ($9<<6) | (me<<1));
	}
	| OP_insrwi            c GPR ',' GPR ',' u5 ',' u5
	{
		quad sh = (32 - $9 - $7) & 0x1f;
		quad me = ($9 + $7 - 1) & 0x1f;
		fit($7 > 0);
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      (sh<<11) | ($9<<6) | (me<<1));
	}
	| OP_rotrwi      c GPR ',' GPR ',' u5
	{
		quad sh = (32 - $7) & 0x1f;
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      (sh<<11) | (0<<6) | (31<<1));
	}
	| OP_slwi              c GPR ',' GPR ',' u5
	{
		quad me = 31 - $7;
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      ($7<<11) | (0<<6) | (me<<1));
	}
	| OP_srwi              c GPR ',' GPR ',' u5
	{
		quad sh = (32 - $7) & 0x1f;
		emit4($1 | $2 | ($5<<21) | ($3<<16) |
		      (sh<<11) | ($7<<6) | (31<<1));
	}
	;

c
	: /* nothing */                          { $$ = 0; }
	| C                                      { $$ = 1; }
	;

e16
	: absexp
	{
		/* Allow signed or unsigned 16-bit values. */
		if (($1 < -0x8000) || ($1 > 0xffff))
			serror("16-bit value out of range");
		$$ = (uint16_t) $1;
	}
	| OP_HI ASC_LPAR expr ASC_RPAR           { $$ = emit_hi(&$3, false); }
	| OP_HA ASC_LPAR expr ASC_RPAR           { $$ = emit_hi(&$3, true); }
	| OP_LO ASC_LPAR expr ASC_RPAR           { $$ = emit_lo(&$3); }
	;

negate16
	: absexp
	{
		/* To encode subi, we negate the immediate value, then
		 * it must fit as signed 16-bit. */
		$$ = -$1;
		fit(fitx($$, 16));
		$$ = (uint16_t) $$;
	}
	;

u8
	: absexp
	{
		if (($1 < 0) || ($1 > 0xFF))
			serror("8-bit unsigned value out of range");
		$$ = $1;
	}
	;

u7
	: absexp
	{
		if (($1 < 0) || ($1 > 0x7F))
			serror("7-bit unsigned value out of range");
		$$ = $1;
	}
	;

u6
	: absexp
	{
		if (($1 < 0) || ($1 > 0x3F))
			serror("6-bit unsigned value out of range");
		$$ = $1;
	}
	;

u5
	: absexp
	{
		if (($1 < 0) || ($1 > 0x1F))
			serror("5-bit unsigned value out of range");
		$$ = $1;
	}
	;

u4
	: absexp
	{
		if (($1 < 0) || ($1 > 0xF))
			serror("4-bit unsigned value out of range");
		$$ = $1;
	}
	;

u1
	: absexp
	{
		if (($1 < 0) || ($1 > 1))
			serror("1-bit unsigned value out of range");
		$$ = $1;
	}
	;

u2
	: absexp
	{
		if (($1 < 0) || ($1 > 0x3))
			serror("2-bit unsigned value out of range");
		$$ = $1;
	}
	;

/* Optional comma, branch hint. */
opt_bh
	: /* nothing */         { $$ = 0; }
	| ',' u2                { $$ = ($2<<11); }

/*
 * Optional condition register, comma.  This checks if the token is a
 * CR register name.  This wouldn't work if we allowed CR as a number.
 */
cr_opt
	: /* nothing */         { $$ = 0; }
	| CR ','                { $$ = $1; }

ds
	: e16
	{
		if ($1 & 3)
			serror("value must be 4-aligned");
		$$ = $1;
	}
	;

nb
	: absexp
	{
		if (($1 < 1) || ($1 > 32))
			serror("register count must be in the range 1..32");

		if ($1 == 32)
			$$ = 0;
		else
			$$ = $1;
	}
	;

bdl
	: expr
	{
		int dist = $1.val - DOTVAL;
		fit(fitx(dist, 25));

		if (dist & 0x3)
			serror("jump targets must be 4-aligned");

		DOTVAL += 2;
		newrelo($1.typ, RELO2 | RELPC | FIXUPFLAGS);
		DOTVAL -= 2;
		$$ = dist & 0xFFFD;
	}
	;

bda
	: expr
	{
		int target = $1.val;
		fit(fitx(target, 16));

		if (target & 0x3)
			serror("jump targets must be 4-aligned");

		DOTVAL += 2;
		newrelo($1.typ, RELO2 | FIXUPFLAGS);
		DOTVAL -= 2;
		$$ = target & 0xFFFD;
	}
	;

li32
	: GPR ',' expr
	{
		quad type = $3.typ & S_TYP;
		quad val = $3.val;
		if ((type == S_ABS) && (val <= 0xffff))
			emit4((14<<26) | ($1<<21) | (0<<16)  | val); /* addi */
		else
		{
			newrelo($3.typ, RELOPPC | FIXUPFLAGS);
			emit4((15<<26) | ($1<<21) | (0<<16)  | (val >> 16)); /* addis */
			emit4((24<<26) | ($1<<21) | ($1<<16) | (val & 0xffff)); /* ori */
		}
	}
	;

lil
	: expr
	{
		int dist = $1.val - DOTVAL;
		fit(fitx(dist, 26));

		if (dist & 0x3)
			serror("jump targets must be 4-aligned");

		newrelo($1.typ, RELOPPC | RELPC | FIXUPFLAGS);
		$$ = dist & 0x03FFFFFD;
	}
	;

lia
	: expr
	{
		int target = $1.val;
		fit(fitx(target, 26));

		if (target & 0x3)
			serror("jump targets must be 4-aligned");

		newrelo($1.typ, RELOPPC | FIXUPFLAGS);
		$$ = target & 0x03FFFFFD;
	}
	;

spr_num
	: SPR { $$ = $1; }
	| absexp
	{
		if (($1 < 0) || ($1 > 0x3ff))
			serror("spr number out of range");
		/* mfspr, mtspr swap the low and high 5 bits */
		$$ = ($1 >> 5) | (($1 & 0x1f) << 5);
	}
	;
