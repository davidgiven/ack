/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Header$"

/*
 * INTEL 8086 special routines
 */

ea_1(param) {

	if ((mrg_1 & 070) || (param & ~070))
		serror("bad operand");
	emit1(mrg_1 | param);
	switch(mrg_1 >> 6) {
	case 0:
		if (mrg_1 == 6 || (mrg_1 & 040)) {
#ifdef RELOCATION
			RELOMOVE(relonami, rel_1);
			newrelo(exp_1.typ, RELO2);
#endif
			emit2(exp_1.val);
		}
		break;
	case 1:
#ifdef RELOCATION
		RELOMOVE(relonami, rel_1);
		newrelo(exp_1.typ, RELO1);
#endif
		emit1(exp_1.val);
		break;
	case 2:
#ifdef RELOCATION
		RELOMOVE(relonami, rel_1);
		newrelo(exp_1.typ, RELO2);
#endif
		emit2(exp_1.val);
		break;
	}
}

ea_2(param) {

	mrg_1 = mrg_2;
	exp_1 = exp_2;
	RELOMOVE(rel_1, rel_2);
	ea_1(param);
}

reverse() {
	register m, r; expr_t e;

	m = mrg_1; mrg_1 = mrg_2; mrg_2 = m;
	e = exp_1; exp_1 = exp_2; exp_2 = e;
#ifndef ASLD
	r = rel_1; rel_1 = rel_2; rel_2 = r;
#endif
}

badsyntax() {

	serror("bad operands");
}

regsize(sz) register sz; {
	register bit;

	sz <<= 3;
	bit = 010;
	sz &= bit;
	if ((mrg_1 >= 0300 && (mrg_1 & bit) != sz) ||
	    (mrg_2 >= 0300 && (mrg_2 & bit) != sz))
		serror("register error");
	mrg_1 &= ~bit;
	mrg_2 &= ~bit;
}

indexed() {

	if (mrg_2 & ~7)
		serror("register error");
	if (exp_2.typ != S_ABS || fitb(exp_2.val) == 0)
		mrg_2 |= 0200;
	else if (exp_2.val != 0 || mrg_2 == 6)
		mrg_2 |= 0100;
}

branch(opc,exp) register opc; expr_t exp; {
	register sm,dist;
	int saving = opc == 0353 ? 1 : 3;

	dist = exp.val - (DOTVAL + 2);
	if (pass == PASS_2 && dist > 0 && !(exp.typ & S_DOT))
		dist -= DOTGAIN;
	sm = dist > 0 ? fitb(dist-saving) : fitb(dist);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if ((opc & 0370) == 0340) {
		fit(sm);
		sm = 1;
	} else {
		if ((sm = small(sm,saving)) == 0) {
			if (opc != 0353) {
				emit1(opc^1);
				emit1(3);
				dist -= 2;
			}
			opc = 0351;
			dist--;
		}
	}
	emit1(opc);
	if (sm == 0) {
#ifdef RELOCATION
		newrelo(exp.typ, RELPC | RELO2);
#endif
		emit2(dist);
	} else
		emit1(dist);
}

pushop(opc) register opc; {

	regsize(1);
	if (mrg_1 & 020) {
		if ( (mrg_1&3) == 1 && opc==1 ) badsyntax() ;
		emit1(6 | opc | (mrg_1&3)<<3);
	} else if (mrg_1 >= 0300) {
		emit1(0120 | opc<<3 | (mrg_1&7));
	} else if (opc == 0) {
		emit1(0377); ea_1(6<<3);
	} else {
		emit1(0217); ea_1(0<<3);
	}
}

addop(opc) register opc; {

	regsize(opc);
	if (mrg_2 >= 0300) {
		emit1(opc); ea_1((mrg_2&7)<<3);
	} else if ((mrg_2 & 040) && mrg_1 == 0300) {
		emit1(opc | 4);
#ifdef RELOCATION
		RELOMOVE(relonami, rel_2);
		newrelo(exp_2.typ, (opc&1)? RELO2 : RELO1);
#endif
		emitx(exp_2.val, (opc&1)+1);
	} else if (mrg_2 & 040) {
		if ((opc&1) == 0) {
			emit1(0200);
		} else if (exp_2.typ != S_ABS || fitb(exp_2.val) == 0 ||
			   opc==011 || opc==041 || opc==061 ) {
			emit1(0201);
		} else {
			emit1(0203); opc &= ~1;
		}
		ea_1(opc & 070);
#ifdef RELOCATION
			RELOMOVE(relonami, rel_2);
			newrelo(exp_2.typ, (opc&1)? RELO2 : RELO1);
#endif
		emitx(exp_2.val, (opc&1)+1);
	} else if (mrg_1 >= 0300) {
		emit1(opc | 2);
		ea_2((mrg_1&7)<<3);
	} else
		badsyntax();
}

rolop(opc) register opc; {
	register cmrg;

	cmrg = mrg_2;
	mrg_2 = mrg_1;
	regsize(opc);
	if (cmrg == 0301) {
		emit1(0322 | (opc&1)); ea_1(opc&070);
	} else if ((cmrg & 040) && exp_2.val == 1) {
		emit1(0320 | (opc&1)); ea_1(opc&070);
	} else
		badsyntax();
}

incop(opc) register opc; {

	regsize(opc);
	if ((opc&1) && mrg_1>=0300) {
		emit1(0100 | (opc&010) | (mrg_1&7));
	} else {
		emit1(0376 | (opc&1));
		ea_1(opc & 010);
	}
}

callop(opc) register opc; {

	regsize(1);
	if (mrg_1 & 040) {
		if (opc == (040+(0351<<8))) {
			RELOMOVE(relonami, rel_1);
			branch(0353,exp_1);
		} else {
			exp_1.val -= (DOTVAL+3);
			emit1(opc>>8);
#ifdef RELOCATION
			RELOMOVE(relonami, rel_1);
			newrelo(exp_1.typ, RELPC | RELO2);
#endif
			emit2(exp_1.val);
		}
	} else {
		emit1(0377); ea_1(opc&070);
	}
}

xchg(opc) register opc; {

	regsize(opc);
	if (mrg_2 == 0300 || mrg_1 < 0300)
		reverse();
	if (opc == 1 && mrg_1 == 0300 && mrg_2 >= 0300) {
		emit1(0220 | (mrg_2&7));
	} else if (mrg_1 >= 0300) {
		emit1(0206 | opc); ea_2((mrg_1&7)<<3);
	} else
		badsyntax();
}

test(opc) register opc; {

	regsize(opc);
	if ((mrg_1 & 040) || mrg_2 >= 0300)
		reverse();
	if ((mrg_2 & 040) && mrg_1 == 0300) {
		emit1(0250 | opc);
#ifdef RELOCATION
			RELOMOVE(relonami, rel_2);
			newrelo(exp_2.typ, (opc&1)? RELO2 : RELO1);
#endif
		emitx(exp_2.val, (opc&1)+1);
	} else if (mrg_2 & 040) {
		emit1(0366 | opc);
		ea_1(0<<3);
#ifdef RELOCATION
			RELOMOVE(relonami, rel_2);
			newrelo(exp_2.typ, (opc&1)? RELO2 : RELO1);
#endif
		emitx(exp_2.val, (opc&1)+1);
	} else if (mrg_1 >= 0300) {
		emit1(0204 | opc); ea_2((mrg_1&7)<<3);
	} else
		badsyntax();
}

mov(opc) register opc; {

	regsize(opc);
	if (mrg_1 & 020) {
		emit1(0216); ea_2((mrg_1&3)<<3);
	} else if (mrg_2 & 020) {
		emit1(0214); ea_1((mrg_2&3)<<3);
	} else if (mrg_2 & 040) {
		if (mrg_1 >= 0300) {
			emit1(0260 | opc<<3 | (mrg_1&7)); 
#ifdef RELOCATION
			RELOMOVE(relonami, rel_2);
			newrelo(exp_2.typ, (opc&1)? RELO2 : RELO1);
#endif
			emitx(exp_2.val, (opc&1)+1);
		} else {
			emit1(0306 | opc); ea_1(0<<3); 
#ifdef RELOCATION
			RELOMOVE(relonami, rel_2);
			newrelo(exp_2.typ, (opc&1)? RELO2 : RELO1);
#endif
			emitx(exp_2.val, (opc&1)+1);
		}
	} else if (mrg_2 == 0300 && mrg_1 == 6) {
		emit1(0242 | opc);
#ifdef RELOCATION
		RELOMOVE(relonami, rel_1);
		newrelo(exp_1.typ, RELO2);
#endif
		emit2(exp_1.val);
	} else if (mrg_1 == 0300 && mrg_2 == 6) {
		emit1(0240 | opc);
#ifdef RELOCATION
		RELOMOVE(relonami, rel_2);
		newrelo(exp_2.typ, RELO2);
#endif
		emit2(exp_2.val);
	} else if (mrg_2 >= 0300) {
		emit1(0210 | opc); ea_1((mrg_2&7)<<3);
	} else if (mrg_1 >= 0300) {
		emit1(0212 | opc); ea_2((mrg_1&7)<<3);
	} else
		badsyntax();
}
