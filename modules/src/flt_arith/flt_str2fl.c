/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include <ctype.h>
#include "flt_misc.h"

/* The following tables can be computed with the following bc(1)
   program:

obase=16
scale=0
define t(x){
	auto a, b, c
	a=2;b=1;c=2^32;n=1
	while(a<x) {
		b=a;n+=n;a*=a
	}
	n/=2
	a=b
	while(b<x) {
		a=b;b*=c;n+=32
	}
	n-=32
	b=a
	while(a<x) {
		b=a;a+=a;n+=1
	}
	n-=1
	x*=16^16
	b=x%a
	x/=a
	if(a<=(2*b)) x+=1
	obase=10
	n
	obase=16
	return(x)
}
for (i=1;i<28;i++) {
	t(10^i)
}
0
for (i=1;i<20;i++) {
	t(10^(28*i))
}
0
define r(x){
	auto a, b, c
	a=2;b=1;c=2^32;n=1
	while(a<x) {
		b=a;n+=n;a*=a
	}
	n/=2
	a=b
	while(b<x) {
		a=b;b*=c;n+=32
	}
	n-=32
	b=a
	while(a<x) {
		b=a;a+=a;n+=1
	}
	a=b
	a*=16^16
	b=a%x
	a/=x
	if(x<=(2*b)) a+=1
	obase=10
	-n
	obase=16
	return(a)
}
for (i=1;i<28;i++) {
	r(10^i)
}
0
for (i=1;i<20;i++) {
	r(10^(28*i))
}
0

*/
static flt_arith s10pow[] = {	/* representation of 10 ** i */
	{ 0,	0,	0x80000000,	0 },
	{ 0,	3,	0xA0000000,	0 },
	{ 0,	6,	0xC8000000,	0 },
	{ 0,	9,	0xFA000000,	0 },
	{ 0,	13,	0x9C400000,	0 },
	{ 0,	16,	0xC3500000,	0 },
	{ 0,	19,	0xF4240000,	0 },
	{ 0,	23,	0x98968000,	0 },
	{ 0,	26,	0xBEBC2000,	0 },
	{ 0,	29,	0xEE6B2800,	0 },
	{ 0,	33,	0x9502F900,	0 },
	{ 0,	36,	0xBA43B740,	0 },
	{ 0,	39,	0xE8D4A510,	0 },
	{ 0,	43,	0x9184E72A,	0 },
	{ 0,	46,	0xB5E620F4,	0x80000000 },
	{ 0,	49,	0xE35FA931,	0xA0000000 },
	{ 0,	53,	0x8E1BC9BF,	0x04000000 },
	{ 0,	56,	0xB1A2BC2E,	0xC5000000 },
	{ 0,	59,	0xDE0B6B3A,	0x76400000 },
	{ 0,	63,	0x8AC72304,	0x89E80000 },
	{ 0,	66,	0xAD78EBC5,	0xAC620000 },
	{ 0,	69,	0xD8D726B7,	0x177A8000 },
	{ 0,	73,	0x87867832,	0x6EAC9000 },
	{ 0,	76,	0xA968163F,	0x0A57B400 },
	{ 0,	79,	0xD3C21BCE,	0xCCEDA100 },
	{ 0,	83,	0x84595161,	0x401484A0 },
	{ 0,	86,	0xA56FA5B9,	0x9019A5C8 },
	{ 0,	89,	0xCECB8F27,	0xF4200F3A }
};
static flt_arith big_10pow[] = {  /* representation of 10 ** (28*i) */
	{ 0,	0,	0x80000000,	0 },
	{ 0,	93,	0x813F3978,	0xF8940984 },
	{ 0,	186,	0x82818F12,	0x81ED44A0 },
	{ 0,	279,	0x83C7088E,	0x1AAB65DB },
	{ 0,	372,	0x850FADC0,	0x9923329E },
	{ 0,	465,	0x865B8692,	0x5B9BC5C2 },
	{ 0,	558,	0x87AA9AFF,	0x79042287 },
	{ 0,	651,	0x88FCF317,	0xF22241E2 },
	{ 0,	744,	0x8A5296FF,	0xE33CC930 },
	{ 0,	837,	0x8BAB8EEF,	0xB6409C1A },
	{ 0,	930,	0x8D07E334,	0x55637EB3 },
	{ 0,	1023,	0x8E679C2F,	0x5E44FF8F },
	{ 0,	1116,	0x8FCAC257,	0x558EE4E6 },
	{ 0,	1209,	0x91315E37,	0xDB165AA9 },
	{ 0,	1302,	0x929B7871,	0xDE7F22B9 },
	{ 0,	1395,	0x940919BB,	0xD4620B6D },
	{ 0,	1488,	0x957A4AE1,	0xEBF7F3D4 },
	{ 0,	1581,	0x96EF14C6,	0x454AA840 },
	{ 0,	1674,	0x98678061,	0x27ECE4F5 },
	{ 0,	1767,	0x99E396C1,	0x3A3ACFF2 }
};

static flt_arith r_10pow[] = { /* representation of 10 ** -i */
	{ 0,	0,	0x80000000,	0 },
	{ 0,	-4,	0xCCCCCCCC,	0xCCCCCCCD },
	{ 0,	-7,	0xA3D70A3D,	0x70A3D70A },
	{ 0,	-10,	0x83126E97,	0x8D4FDF3B },
	{ 0,	-14,	0xD1B71758,	0xE219652C },
	{ 0,	-17,	0xA7C5AC47,	0x1B478423 },
	{ 0,	-20,	0x8637BD05,	0xAF6C69B6 },
	{ 0,	-24,	0xD6BF94D5,	0xE57A42BC },
	{ 0,	-27,	0xABCC7711,	0x8461CEFD },
	{ 0,	-30,	0x89705F41,	0x36B4A597 },
	{ 0,	-34,	0xDBE6FECE,	0xBDEDD5BF },
	{ 0,	-37,	0xAFEBFF0B,	0xCB24AAFF },
	{ 0,	-40,	0x8CBCCC09,	0x6F5088CC },
	{ 0,	-44,	0xE12E1342,	0x4BB40E13 },
	{ 0,	-47,	0xB424DC35,	0x095CD80F },
	{ 0,	-50,	0x901D7CF7,	0x3AB0ACD9 },
	{ 0,	-54,	0xE69594BE,	0xC44DE15B },
	{ 0,	-57,	0xB877AA32,	0x36A4B449 },
	{ 0,	-60,	0x9392EE8E,	0x921D5D07 },
	{ 0,	-64,	0xEC1E4A7D,	0xB69561A5 },
	{ 0,	-67,	0xBCE50864,	0x92111AEB },
	{ 0,	-70,	0x971DA050,	0x74DA7BEF },
	{ 0,	-74,	0xF1C90080,	0xBAF72CB1 },
	{ 0,	-77,	0xC16D9A00,	0x95928A27 },
	{ 0,	-80,	0x9ABE14CD,	0x44753B53 },
	{ 0,	-84,	0xF79687AE,	0xD3EEC551 },
	{ 0,	-87,	0xC6120625,	0x76589DDB },
	{ 0,	-90,	0x9E74D1B7,	0x91E07E48 }
};

static flt_arith r_big_10pow[] = { /* representation of 10 ** -(28*i) */
	{ 0,	0,	0x80000000,	0 },
	{ 0,	-94,	0xFD87B5F2,	0x8300CA0E },
	{ 0,	-187,	0xFB158592,	0xBE068D2F },
	{ 0,	-280,	0xF8A95FCF,	0x88747D94 },
	{ 0,	-373,	0xF64335BC,	0xF065D37D },
	{ 0,	-466,	0xF3E2F893,	0xDEC3F126 },
	{ 0,	-559,	0xF18899B1,	0xBC3F8CA2 },
	{ 0,	-652,	0xEF340A98,	0x172AACE5 },
	{ 0,	-745,	0xECE53CEC,	0x4A314EBE },
	{ 0,	-838,	0xEA9C2277,	0x23EE8BCB },
	{ 0,	-931,	0xE858AD24,	0x8F5C22CA },
	{ 0,	-1024,	0xE61ACF03,	0x3D1A45DF },
	{ 0,	-1117,	0xE3E27A44,	0x4D8D98B8 },
	{ 0,	-1210,	0xE1AFA13A,	0xFBD14D6E },
	{ 0,	-1303,	0xDF82365C,	0x497B5454 },
	{ 0,	-1396,	0xDD5A2C3E,	0xAB3097CC },
	{ 0,	-1489,	0xDB377599,	0xB6074245 },
	{ 0,	-1582,	0xD91A0545,	0xCDB51186 },
	{ 0,	-1675,	0xD701CE3B,	0xD387BF48 },
	{ 0,	-1768,	0xD4EEC394,	0xD6258BF8 }
};

#define BIGSZ	(sizeof(big_10pow)/sizeof(big_10pow[0]))
#define SMALLSZ	(sizeof(s10pow)/sizeof(s10pow[0]))

static
add_exponent(e, exp)
	register flt_arith *e;
{
	int neg = exp < 0;
	int divsz, modsz;
	flt_arith x;
	register int status = 0;

	if (neg) exp = -exp;
	divsz = exp / SMALLSZ;
	modsz = exp % SMALLSZ;
	if (!status) status = flt_status;
	flt_mul(e, (neg ? r_10pow : s10pow) + modsz, &x);
	if (!status) status = flt_status;
	while (divsz >= BIGSZ) {
		flt_mul(&x, neg ? &r_big_10pow[BIGSZ-1] : &big_10pow[BIGSZ-1],&x);
		if (!status) status = flt_status;
		divsz -= BIGSZ-1;
	}
	flt_mul(&x, (neg ? r_big_10pow : big_10pow) + divsz, e);
	if (!status) status = flt_status;
	flt_status = status;
}

void
flt_str2flt(s, e)
	register char		*s;
	register flt_arith	*e;
{
	register int	c;
	int		dotseen = 0;
	int		digitseen = 0;
	int		exp = 0;

	while (isspace(*s)) s++;

	flt_status = 0;

	e->flt_sign = 0;
	e->flt_exp = 0;
	e->m1 = e->m2 = 0;

	c = *s;
	switch(c) {
	case '-':
		e->flt_sign = 1;
	case '+':
		s++;
	}
	while (c = *s++, isdigit(c) || (c == '.' && ! dotseen++)) {
		if (c == '.') continue;
		digitseen = 1;
		if (e->m1 >= 0 && e->m1 <= 0x7FFFFFFF/5) {
			struct flt_mantissa	a1;

			a1 = e->flt_mantissa;
			flt_b64_sft(&(e->flt_mantissa), -3);
			flt_b64_sft(&a1, -1);
			flt_b64_add(&(e->flt_mantissa), &a1);
			a1.flt_h_32 = 0;
			a1.flt_l_32 = c - '0';
			flt_b64_add(&(e->flt_mantissa), &a1);
		}
		else exp++;
		if (dotseen) exp--;
	}
	if (! digitseen) {
		flt_status = FLT_NOFLT;
		return;
	}

	if (c == 'E' || c == 'e') {
		int	exp1 = 0;
		int	sign = 1;

		switch(*s) {
		case '-':
			sign = -1;
		case '+':
			s++;
		}
		if (c = *s, isdigit(c)) {
			do {
				exp1 = 10 * exp1 + (c - '0');
			} while (c = *++s, isdigit(c));
		}
		exp += sign * exp1;
	}
	if (e->m1 == 0 && e->m2 == 0) return;
	e->flt_exp = 63;
	flt_nrm(e);
	add_exponent(e, exp);
	flt_chk(e);
}

#define NDIG 18

static char *
flt_ecvt(e, decpt, sign)
	register flt_arith *e;
	int *decpt, *sign;
{
	/*	Like ecvt(), but for extended precision */

	static char buf[NDIG+1];
	register char *p = buf;
	register char *pe;
	register int findex = 0;

	pe = &buf[NDIG];
	buf[0] = '\0';

	*sign = 0;
	if (e->flt_sign) {
		*sign = 1;
		e->flt_sign = 0;
	}

	*decpt = 0;
	if (e->m1 != 0) {
		register flt_arith *pp = &big_10pow[1];

		findex = 1;
		while (flt_cmp(e, &big_10pow[BIGSZ-1]) >= 0) {
			flt_mul(e,&r_big_10pow[BIGSZ-1],e);
			*decpt += (BIGSZ-1)*SMALLSZ;
		}
		while (flt_cmp(e,pp) >= 0) {
			pp++;
			findex++;
		}
		findex--;
		flt_mul(e,&r_big_10pow[findex],e);
		*decpt += findex*SMALLSZ;
		pp = &s10pow[1];
		findex = 1;
		while (pp < &s10pow[SMALLSZ] && flt_cmp(e, pp) >= 0) {
			pp++;
			findex++;
		}
		findex--;
		*decpt += findex;

		if (flt_cmp(e, &s10pow[0]) < 0) {
                	while (flt_cmp(e, &r_big_10pow[BIGSZ-1]) < 0) { 
                        	flt_mul(e,&big_10pow[BIGSZ-1],e);
                        	*decpt -= (BIGSZ-1)*SMALLSZ;
                	}
			pp = &r_big_10pow[1];
			findex = 1;
			while(flt_cmp(e,pp) < 0) {
				pp++;
				findex++;
			}
			findex--;
			flt_mul(e,&big_10pow[findex],e);
			*decpt -= findex*SMALLSZ;
			/* here, value >= 10 ** -28 */
			flt_mul(e, &s10pow[1], e);
			(*decpt)--;
			pp = &r_10pow[0];
			findex = 0;
			while(flt_cmp(e, pp) < 0) {
				pp++;
				findex++;
			}
			flt_mul(e, &s10pow[findex], e);
			*decpt -= findex;
			findex = 0;
		}
		(*decpt)++;	/* because now value in [1.0, 10.0) */
	}
	while (p <= pe) {
		if (findex) {
			flt_arith tc, oldtc;
			int count = 0;

			oldtc.flt_exp = 0;
			oldtc.flt_sign = 0;
			oldtc.m1 = 0;
			oldtc.m2 = 0;
			tc = s10pow[findex];
			while (flt_cmp(e, &tc) >= 0) {
				oldtc = tc;
				flt_add(&tc, &s10pow[findex], &tc);
				count++;
			}
			*p++ = count + '0';
			oldtc.flt_sign = 1;
			flt_add(e, &oldtc, e);
			findex--;
			continue;
		}
		if (e->flt_exp >= 0 && e->m1 != 0) {
			flt_arith x;

			x.m2 = 0; x.flt_exp = e->flt_exp;
			x.flt_sign = 1;
			x.m1 = (e->m1 >> 1) & 0x7FFFFFFF;
			x.m1 = x.m1>>(30-e->flt_exp);
			*p++ = (x.m1) + '0';
			if (x.m1) {
				x.m1 = x.m1 << (31-e->flt_exp);
				flt_add(e, &x, e);
			}
		}
		else *p++ = '0';
		if (e->m1) flt_mul(e, &s10pow[1], e);
	}
	if (pe >= buf) {
		p = pe;
		*p += 5;	/* round of at the end */
		while (*p > '9') {
			*p = '0';
			if (p > buf) ++*--p;
			else {
				*p = '1';
				++*decpt;
			}
		}
		*pe = '\0';
		while (--pe > buf && *pe == '0') *pe = '\0';
	}
	return buf;
}

void
flt_flt2str(e, buf, bufsize)
	flt_arith *e;
	char *buf;
{

	int sign, dp;
	register int i;
	register char *s1;
	char Xbuf[NDIG+12];
	register char *s = Xbuf;
	flt_arith e1;

	e1 = *e;
	flt_status = 0;
	s1 = flt_ecvt(&e1,&dp,&sign);
        if (sign)
                *s++ = '-';
        *s++ = *s1++;
        *s++ = '.';
        for (i = NDIG-1; i > 0; i--) {
                if (*s1) *s++ = *s1++;
                else {
			if (i == NDIG-1) *s++ = '0';
			break;
		}
	}
        if (e->m1 | e->m2) {
		--dp ;
	}
	if (dp != 0) {
        	*s++ = 'e';
        	if ( dp<0 ) {
                	*s++ = '-' ; dp= -dp ;
        	} else {
                	*s++ = '+' ;
        	}
		s1 = &Xbuf[NDIG+12];
		*--s1 = '\0';
		do {
			*--s1 = dp % 10 + '0';
			dp /= 10;
		} while (dp != 0);
		while (*s1) *s++ = *s1++;
	}
	*s++ = '\0';
	if (s - Xbuf > bufsize) {
		flt_status = FLT_BTSM;
		return;
	}
	s = Xbuf;
	s1 = buf;
	do {
		*s1++ = *s;
	} while (*s++);
}
